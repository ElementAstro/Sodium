/*
 *  eegg.cpp
 *  PHD Guiding
 *
 *  Created by Craig Stark.
 *  Copyright (c) 2007-2010 Craig Stark.
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Craig Stark, Stark Labs nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "calibration_assistant.hpp"
#include "calreview_dialog.hpp"
#include "comet_tool.hpp"
#include "drift_tool.hpp"
#include "guiding_assistant.hpp"
#include "manualcal_dialog.hpp"
#include "nudge_lock.hpp"
#include "polardrift_tool.hpp"
#include "sodium.hpp"
#include "staticpa_tool.hpp"


static wxString FlipCalEnabledKey() {
    // we want the key to be under "/Confirm" so
    // ConfirmDialog::ResetAllDontAskAgain() resets it, but we also want the
    // setting to be per-profile
    return wxString::Format("/Confirm/%d/FlipCalWarningEnabled",
                            pConfig->GetCurrentProfileId());
}

static void SuppressFlipCalAlert(long) {
    pConfig->Global.SetBoolean(FlipCalEnabledKey(), false);
}

void MyFrame::OnEEGG(wxCommandEvent& evt) {
    if (evt.GetId() == EEGG_RESTORECAL || evt.GetId() == EEGG_REVIEWCAL) {
        wxString savedCal = pConfig->Profile.GetString(
            "/scope/calibration/timestamp", wxEmptyString);
        if (savedCal.IsEmpty()) {
            wxMessageBox(_("There is no calibration data available."));
            return;
        }

        if (!pMount) {
            wxMessageBox(_("Please connect a mount first."));
            return;
        }

        if (evt.GetId() == EEGG_RESTORECAL) {
            CalRestoreDialog(this).ShowModal();
        } else {
            if (pCalReviewDlg)  // Review dialog is non-modal
                pCalReviewDlg->Destroy();
            pCalReviewDlg = new CalReviewDialog(this);
            pCalReviewDlg->Show();
        }
    } else if (evt.GetId() == EEGG_MANUALCAL) {
        if (pMount) {
            Calibration cal;
            cal.xRate = pMount->xRate();
            cal.yRate = pMount->yRate();
            cal.xAngle = pMount->xAngle();
            cal.yAngle = pMount->yAngle();
            cal.declination = pPointingSource->GetDeclinationRadians();
            cal.pierSide = pPointingSource->SideOfPier();
            cal.raGuideParity = cal.decGuideParity = GUIDE_PARITY_UNCHANGED;
            cal.rotatorAngle = Rotator::RotatorPosition();
            cal.binning = pCamera->Binning;
            cal.isValid = true;

            if (!pMount->IsCalibrated()) {
                cal.xRate = 1.0;
                cal.yRate = 1.0;
                cal.xAngle = 0.0;
                cal.yAngle = M_PI / 2.;
                cal.declination = UNKNOWN_DECLINATION;
            }

            ManualCalDialog manualcal(cal);
            if (manualcal.ShowModal() == wxID_OK) {
                manualcal.GetValues(&cal);
                pMount->SetCalibration(cal);
            }
        }
    } else if (evt.GetId() == EEGG_CLEARCAL) {
        wxString devicestr = "";
        if (!(pGuider && pGuider->IsCalibratingOrGuiding())) {
            if (pMount) {
                if (pMount->IsStepGuider())
                    devicestr = _("AO");
                else
                    devicestr = _("Mount");
            }
            if (pSecondaryMount) {
                devicestr += _(", Mount");
            }
            if (devicestr.Length() > 0) {
                if (wxMessageBox(
                        wxString::Format(
                            _("%s calibration will be cleared - calibration "
                              "will be re-done when guiding is started."),
                            devicestr),
                        _("Clear Calibration"), wxOK | wxCANCEL) == wxOK) {
                    if (pMount)
                        pMount->ClearCalibration();
                    if (pSecondaryMount)
                        pSecondaryMount->ClearCalibration();
                    Debug.AddLine("User cleared calibration on " + devicestr);
                }
            }
        }
    } else if (evt.GetId() == EEGG_FLIPCAL) {
        Mount* scope = TheScope();

        if (scope) {
            double xorig = degrees(scope->xAngle());
            double yorig = degrees(scope->yAngle());

            Debug.AddLine("User-requested FlipCal");
            if (!TheScope()->IsCalibrated()) {
                wxMessageBox(
                    _("Scope has no current calibration data - you should just "
                      "do a fresh calibration."));
            } else {
                Calibration lastCal;
                TheScope()->GetLastCalibration(&lastCal);
                if (pPointingSource->CanReportPosition() &&
                    pPointingSource->SideOfPier() != PIER_SIDE_UNKNOWN &&
                    lastCal.pierSide != PIER_SIDE_UNKNOWN) {
                    pFrame->SuppressableAlert(
                        FlipCalEnabledKey(),
                        _("This is unnecessary because PHD2 has pointing info "
                          "from the mount.  If you are seeing run-away Dec "
                          "guiding "
                          "after a meridian flip, use Help and look in the "
                          "index for 'Reverse Dec output'."),
                        SuppressFlipCalAlert, 0, true);
                } else {
                    if (FlipCalibrationData()) {
                        wxMessageBox(
                            _("Failed to flip calibration - please upload "
                              "debug log file to PHD2 forum for assistance."));
                    } else {
                        double xnew = degrees(scope->xAngle());
                        double ynew = degrees(scope->yAngle());
                        wxMessageBox(
                            wxString::Format(_("Calibration angles flipped: "
                                               "(%.2f, %.2f) to (%.2f, %.2f)"),
                                             xorig, yorig, xnew, ynew));
                    }
                }
            }
        }
    } else if (evt.GetId() == EEGG_MANUALLOCK) {
        if (!pFrame->pNudgeLock)
            pFrame->pNudgeLock = NudgeLockTool::CreateNudgeLockToolWindow();

        pFrame->pNudgeLock->Show();
    } else if (evt.GetId() == EEGG_STICKY_LOCK) {
        bool sticky = evt.IsChecked();
        pFrame->pGuider->SetLockPosIsSticky(sticky);
        pConfig->Global.SetBoolean("/StickyLockPosition", sticky);
        NudgeLockTool::UpdateNudgeLockControls();
    } else {
        evt.Skip();
    }
}

void MyFrame::OnDriftTool(wxCommandEvent& WXUNUSED(evt)) {
    if (!pDriftTool) {
        pDriftTool = DriftTool::CreateDriftToolWindow();
    }

    if (pDriftTool) {
        pDriftTool->Show();
    }
}

void MyFrame::OnPolarDriftTool(wxCommandEvent& WXUNUSED(evt)) {
    if (!pPolarDriftTool) {
        pPolarDriftTool = PolarDriftTool::CreatePolarDriftToolWindow();
    }

    if (pPolarDriftTool) {
        pPolarDriftTool->Show();
    }
}

void MyFrame::OnCalibrationAssistant(wxCommandEvent& WXUNUSED(evt)) {
    if (!pCalibrationAssistant)
        pCalibrationAssistant =
            CalibrationAssistantFactory::MakeCalibrationAssistant();
    pCalibrationAssistant->Show();
}

void MyFrame::OnServerAssistant(wxCommandEvent& WXUNUSED(evt)) {
    if (!pServerAssistant) {
        pServerAssistant = ServerAssistantFactory::MakeServerAssistant();
    }
    pServerAssistant->Show();
}

void MyFrame::OnSolverAssistant(wxCommandEvent& WXUNUSED(evt)) {
    if (!pSolverAssistant) {
        pSolverAssistant = SolverAssistantFactory::MakeSolverAssistant();
    }
    pSolverAssistant->Show();
}

void MyFrame::OnStaticPaTool(wxCommandEvent& WXUNUSED(evt)) {
    if (!pStaticPaTool) {
        pStaticPaTool = StaticPaTool::CreateStaticPaToolWindow();
    }

    if (pStaticPaTool) {
        pStaticPaTool->Show();
    }
}

void MyFrame::OnCometTool(wxCommandEvent& WXUNUSED(evt)) {
    if (!pCometTool) {
        pCometTool = CometTool::CreateCometToolWindow();
    }

    if (pCometTool) {
        pCometTool->Show();
    }
}

void MyFrame::OnGuidingAssistant(wxCommandEvent& WXUNUSED(evt)) {
    if (!pCamera) {
        wxMessageBox(_("Please connect a camera first."));
        return;
    }
    if (!TheScope()) {
        wxMessageBox(_("Please connect a mount first."));
        return;
    }

    if (!pGuidingAssistant) {
        bool ok = true;

        if (pFrame->pGuider->IsGuiding()) {
            ok = ConfirmDialog::Confirm(
                _("The Guiding Assitant will disable guide output and\n"
                  "allow the guide star to drift.\n"
                  "\n"
                  "Ok to disable guide output?"),
                "/guiding_assistant_while_guiding",
                _("Confirm Disable Guiding"));
        }

        if (ok)
            pGuidingAssistant = GuidingAssistant::CreateDialogBox();
    }

    if (pGuidingAssistant) {
        pGuidingAssistant->Show();
    }
}
