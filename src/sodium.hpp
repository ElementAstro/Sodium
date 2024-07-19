/*
 * sodium.hpp
 *
 * Copyright (C) 2023-2024 Max Qian <lightapt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SODIUM_H_INCLUDED
#define SODIUM_H_INCLUDED

#if defined(_WINDOWS)
// ensure that windows.h does not include winsock.h which is
// incompatible with winsock2.h
#undef NOMINMAX
#define NOMINMAX
// clang-format off
#include <winsock2.h>
#include <windows.h>
// clang-format on
#endif

#include <wx/aui/aui.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/config.h>
#include <wx/dcbuffer.h>
#include <wx/display.h>
#include <wx/ffile.h>
#include <wx/fileconf.h>
#include <wx/graphics.h>
#include <wx/grid.h>
#include <wx/html/helpctrl.h>
#include <wx/image.h>
#include <wx/infobar.h>
#include <wx/intl.h>
#include <wx/minifram.h>
#include <wx/msgqueue.h>
#include <wx/socket.h>
#include <wx/spinctrl.h>
#include <wx/splash.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/textfile.h>
#include <wx/tglbtn.h>
#include <wx/thread.h>
#include <wx/utils.h>
#include <wx/wx.h>

#include <cmath>
#include <cstdarg>
#include <functional>
#include <map>

WX_DEFINE_ARRAY_INT(int, ArrayOfInts);
WX_DEFINE_ARRAY_DOUBLE(double, ArrayOfDbl);

#include "macro.hpp"

// clang-format off
#include "config.hpp"
#include "configdialog.hpp"
#include "optionsbutton.hpp"
#include "usImage.hpp"
#include "point.hpp"
#include "star.hpp"
#include "circbuf.hpp"
#include "guidinglog.hpp"
#include "graph.hpp"
#include "statswindow.hpp"
#include "star_profile.hpp"
#include "target.hpp"
#include "graph-stepguider.hpp"
#include "guide_algorithms.hpp"
#include "guiders.hpp"
#include "messagebox_proxy.hpp"
#include "serialports.hpp"
#include "parallelports.hpp"
#include "onboard_st4.hpp"
#include "cameras.hpp"
#include "camera.hpp"
#include "mount.hpp"
#include "scopes.hpp"
#include "stepguiders.hpp"
#include "rotators.hpp"
#include "image_math.hpp"
#include "testguide.hpp"
#include "advanced_dialog.hpp"
#include "gear_dialog.hpp"
#include "myframe.hpp"
#include "debuglog.hpp"
#include "worker_thread.hpp"
#include "event_server.hpp"
#include "confirm_dialog.hpp"
#include "controller.hpp"
#include "runinbg.hpp"
#include "fitsiowrap.hpp"
#include "imagelogger.hpp"
#include "server_dialog.hpp"
#include "solver_dialog.hpp"
// clang-format on

class wxSingleInstanceChecker;

extern Mount *pMount;
extern Mount *pSecondaryMount;
extern Scope *pPointingSource;  // For using an 'aux' mount connection to get
                                // pointing info if the user has specified one
extern GuideCamera *pCamera;

inline static Scope *TheScope() {
    return static_cast<Scope *>(
        pMount && pMount->IsStepGuider() ? pSecondaryMount : pMount);
}

inline static StepGuider *TheAO() {
    return static_cast<StepGuider *>(pMount && pMount->IsStepGuider() ? pMount
                                                                      : 0);
}

// these seem to be the windowing/display related globals
extern int XWinSize;
extern int YWinSize;

class PhdApp : public wxApp {
    wxSingleInstanceChecker *m_instanceChecker;
    long m_instanceNumber;
    bool m_resetConfig;
    wxString m_resourcesDir;
    wxDateTime m_logFileTime;

protected:
    wxLocale m_locale;

public:
    PhdApp();
    bool OnInit();
    int OnExit();
    void OnInitCmdLine(wxCmdLineParser &parser);
    bool OnCmdLineParsed(wxCmdLineParser &parser);
    void TerminateApp();
    void RestartApp();
    void HandleRestart();
    void ResetConfiguration();
    virtual bool Yield(bool onlyIfNeeded = false);
    static void ExecInMainThread(std::function<void()> func);
    int GetInstanceNumber() const { return m_instanceNumber; }
    const wxString &GetPHDResourcesDir() const { return m_resourcesDir; }
    wxString GetLocalesDir() const;
    const wxLocale &GetLocale() const { return m_locale; }
    const wxDateTime &GetLogFileTime() const { return m_logFileTime; }
    static wxDateTime ImagingDay(const wxDateTime &dt);
    static bool IsSameImagingDay(const wxDateTime &a, const wxDateTime &b);
    void CheckLogRollover();
    wxString UserAgent() const;
};

wxDECLARE_APP(PhdApp);

#endif  // SODIUM_H_INCLUDED
