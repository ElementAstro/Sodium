/*
 *  socket_server.cpp
 *  LGuider Guiding
 *
 *  Created by Craig Stark.
 *  Copyright (c) 2008-2010 Craig Stark.
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

#include "lightguider.h"

#include "ao/gear_simulator.h"
#include "socket_server.h"


#include <spdlog/spdlog.h>

#include <algorithm>
#include <functional>

static std::set<wxSocketBase *> s_clients;

enum {
    MSG_PAUSE = 1,
    MSG_RESUME,
    MSG_MOVE1,
    MSG_MOVE2,
    MSG_MOVE3,
    MSG_IMAGE,
    MSG_GUIDE,
    MSG_CAMCONNECT,
    MSG_CAMDISCONNECT,
    MSG_REQDIST,
    MSG_REQFRAME,
    MSG_MOVE4,
    MSG_MOVE5,
    MSG_AUTOFINDSTAR,
    MSG_SETLOCKPOSITION,  // 15
    MSG_FLIPRACAL,        // 16
    MSG_GETSTATUS,        // 17
    MSG_STOP,             // 18
    MSG_LOOP,             // 19
    MSG_STARTGUIDING,     // 20
    MSG_LOOPFRAMECOUNT,   // 21
    MSG_CLEARCAL,         // 22
    MSG_FLIP_SIM_CAMERA,  // 23
    MSG_DESELECT,         // 24
};

void MyFrame::OnServerMenu(wxCommandEvent &evt) {
    SetServerMode(evt.IsChecked());
    StartServer(GetServerMode());
}

bool MyFrame::StartServer(bool state) {
    try {
        if (state) {
            if (SocketServer) {
                Debug.AddLine("start server, server already running");
                return false;
            }
            Debug.AddLine("starting server");
            // 获取应用程序实例数
            int instanceNumber = wxGetApp().GetInstanceNumber();
            // 计算端口号
            unsigned int port = 4300 + instanceNumber - 1;
            // 创建 SocketServer 对象
            wxIPV4address sockServerAddr;
            sockServerAddr.Service(port);
            SocketServer =
                new wxSocketServer(sockServerAddr, wxSOCKET_REUSEADDR);
            // 判断 SocketServer 是否启动成功
            if (!SocketServer->Ok()) {
                Debug.AddLine(
                    wxString::Format("Socket server failed to start - Could "
                                     "not listen at port %u",
                                     port));
                delete SocketServer;
                SocketServer = nullptr;
                StatusMsg(_("Server start failed"));
                return true;
            }
            // 设置 SocketServer 的事件处理程序
            SocketServer->SetEventHandler(*this, SOCK_SERVER_ID);
            SocketServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
            SocketServer->Notify(true);
            // 启动事件服务器
            if (EvtServer.EventServerStart(instanceNumber)) {
                delete SocketServer;
                SocketServer = nullptr;
                StatusMsg(_("Server start failed"));
                return true;
            }
            Debug.AddLine(
                wxString::Format("Socket server started at port %u", port));
            StatusMsg(_("Server started"));
        } else {
            if (!SocketServer) {
                Debug.AddLine("stop server, server already stopped");
                return false;
            }
            Debug.AddLine("stopping server");
            // 销毁所有客户端连接
            std::for_each(s_clients.begin(), s_clients.end(),
                          std::mem_fn(&wxSocketBase::Destroy));
            bool isClientsEmpty = s_clients.empty();
            // 停止事件服务器
            EvtServer.EventServerStop();
            // 销毁 SocketServer 对象
            delete SocketServer;
            SocketServer = nullptr;
            Debug.AddLine("Socket server stopped");
            StatusMsg(_("Server stopped"));
        }
        return false;
    } catch (std::exception &ex) {
        Debug.AddLine(wxString::Format("Exception caught: %s", ex.what()));
        return true;
    }
}

/*
 * Here's what the code does:
 * 1. Cast the socket event's socket to wxSocketServer*.
 * 2. Check if the socket server is valid.
 * 3. Check if the socket event is wxSOCKET_CONNECTION.
 * 4. Accept the new client connection.
 * 5. If accepting the connection was successful, set up the client's event
 * handler and notification flags.
 * 6. Add the client to the set of connected clients.
 * 7. If any exceptions occur, log them with spdlog.
 */
void MyFrame::OnSockServerEvent(wxSocketEvent &event) {
    try {
        wxSocketServer *server =
            static_cast<wxSocketServer *>(event.GetSocket());

        if (!server) {
            spdlog::error("Invalid socket server");
            return;
        }

        if (event.GetSocketEvent() != wxSOCKET_CONNECTION) {
            spdlog::warn(
                "Unexpected socket server event: expected {}, got {}. Ignoring "
                "it.",
                wxSOCKET_CONNECTION, event.GetSocketEvent());
            return;
        }

        wxSocketBase *client = server->Accept(false);

        if (client) {
            pFrame->StatusMsg(_("New server connection"));
            spdlog::info("New server connection");
        } else {
            spdlog::error("Failed to accept new client connection");
            return;
        }

        client->SetEventHandler(*this, SOCK_SERVER_CLIENT_ID);
        client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
        client->Notify(true);

        s_clients.insert(client);
    } catch (const std::exception &e) {
        spdlog::error("Exception caught in MyFrame::OnSockServerEvent(): {}",
                      e.what());
    } catch (...) {
        spdlog::error(
            "Unknown exception caught in MyFrame::OnSockServerEvent()");
    }
}

double MyFrame::GetDitherAmount(int ditherType) {
    switch (ditherType) {
        case 1:
            return 0.5;
        case 2:
            return 1.0;
        case 3:
            return 2.0;
        case 4:
            return 3.0;
        case 5:
            return 5.0;
    }
    return 1.0;
}

void MyFrame::HandleSockServerInput(wxSocketBase *sock) {
    unsigned char rval = 0;

    try {
        // We disable input events, so that the test doesn't trigger
        // wxSocketEvent again.
        sock->SetNotify(wxSOCKET_LOST_FLAG);

        // Which command is coming in?
        unsigned char c;

        sock->Read(&c, 1);

        Debug.Write(wxString::Format("read socket command %d\n", c));

        switch (c) {
            case MSG_PAUSE:
            case 'p':
                Debug.AddLine("processing socket request PAUSE");
                SetPaused(PAUSE_GUIDING);
                break;

            case MSG_RESUME:
            case 'r':
                Debug.AddLine("processing socket request RESUME");
                SetPaused(PAUSE_NONE);
                break;

            case MSG_MOVE1:  // +/- 0.5
            case MSG_MOVE2:  // +/- 1.0
            case MSG_MOVE3:  // +/- 2.0
            case MSG_MOVE4:  // +/- 3.0
            case MSG_MOVE5:  // +/- 5.0
            {
                Debug.AddLine("processing socket request MOVEn");

                int ditherType = 3;
                switch (c) {
                    case MSG_MOVE1:
                        ditherType = 1;
                        break;
                    case MSG_MOVE2:
                        ditherType = 2;
                        break;
                    case MSG_MOVE3:
                        ditherType = 3;
                        break;
                    case MSG_MOVE4:
                        ditherType = 4;
                        break;
                    case MSG_MOVE5:
                        ditherType = 5;
                        break;
                }

                double size = GetDitherAmount(ditherType);

                wxString errMsg;
                bool ok = PhdController::DitherCompat(size, &errMsg);
                if (!ok) {
                    throw ERROR_INFO(+errMsg);
                }

                rval = RequestedExposureDuration() / 1000;
                if (rval < 1) {
                    rval = 1;
                }
                break;
            }

            case MSG_REQDIST: {
                Debug.AddLine("processing socket request REQDIST");
                if (!pGuider->IsGuiding()) {
                    throw ERROR_INFO("cannot request distance if not guiding");
                }

                double currentError = CurrentGuideError();

                if (currentError > 2.55) {
                    rval = 255;
                } else {
                    rval = (unsigned char)(currentError * 100);
                }

                Debug.AddLine(
                    wxString::Format("SOCKSVR: Sending pixel error of %.2f",
                                     (float)rval / 100.0));
                break;
            }

            case MSG_AUTOFINDSTAR:
                Debug.AddLine("processing socket request AUTOFINDSTAR");
                bool error;
                error = pFrame->AutoSelectStar();
                rval = error ? 0 : 1;
                if (!error) {
                    StartLooping();
                }
                GuideLog.ServerCommand(pGuider, "AUTO FIND STAR");
                break;

            case MSG_SETLOCKPOSITION: {
                // Sets LockX and LockY to be user-specified
                unsigned short x, y;
                sock->Read(&x, 2);
                sock->Read(&y, 2);
                sock->Discard();  // Clean out anything else

                if (!pFrame->pGuider->SetLockPosToStarAtPosition(
                        LGuider_Point(x, y))) {
                    Debug.Write(wxString::Format(
                        "processing socket request SETLOCKPOSITION for (%d, "
                        "%d) succeeded\n",
                        x, y));
                    pFrame->StatusMsg(
                        wxString::Format(_("Lock set to %d,%d"), x, y));
                    GuideLog.NotifySetLockPosition(pGuider);
                } else {
                    Debug.Write(wxString::Format(
                        "processing socket request SETLOCKPOSITION for (%d, "
                        "%d) failed\n",
                        x, y));
                }
                break;
            }

            case MSG_FLIPRACAL: {
                Debug.AddLine("processing socket request FLIPRACAL");
                PauseType prev = pGuider->SetPaused(PAUSE_GUIDING);
                // return 1 for success, 0 for failure
                rval = 1;
                if (FlipCalibrationData()) {
                    rval = 0;
                }
                pGuider->SetPaused(prev);
                GuideLog.ServerCommand(pGuider, "FLIP RA CAL");
                break;
            }

            case MSG_GETSTATUS:
                Debug.AddLine("processing socket request GETSTATUS");
                rval = Guider::GetExposedState();
                break;

            case MSG_LOOP: {
                Debug.AddLine("processing socket request LOOP");
                StartLooping();
                GuideLog.ServerCommand(pGuider, "LOOP");
                break;
            }

            case MSG_STOP: {
                Debug.AddLine("processing socket request STOP");
                StopCapturing();
                GuideLog.ServerCommand(pGuider, "STOP");
                break;
            }

            case MSG_STARTGUIDING: {
                Debug.AddLine("processing socket request STARTGUIDING");
                bool err = StartGuiding();
                Debug.AddLine(wxString::Format(
                    "StartGuiding returned %d, guider state is %d", err,
                    pFrame->pGuider->GetState()));
                GuideLog.ServerCommand(pGuider, "START GUIDING");
                break;
            }

            case MSG_LOOPFRAMECOUNT:
                Debug.AddLine("processing socket request LOOPFRAMECOUNT");
                if (!CaptureActive) {
                    rval = 0;
                } else if (m_frameCounter > UCHAR_MAX) {
                    rval = UCHAR_MAX;
                } else {
                    rval = m_frameCounter;
                }
                break;

            case MSG_CLEARCAL:
                Debug.AddLine("processing socket request CLEARCAL");

                if (pMount)
                    pMount->ClearCalibration();
                if (pSecondaryMount)
                    pSecondaryMount->ClearCalibration();

                GuideLog.ServerCommand(pGuider, "CLEAR CAL");
                break;

            case MSG_FLIP_SIM_CAMERA:
                Debug.AddLine(
                    "processing socket request flip camera simulator");
                GearSimulator::FlipPierSide(pCamera);
                break;

            case MSG_DESELECT:
                Debug.AddLine("processing socket request deselect");
                pFrame->pGuider->Reset(true);
                break;

            default:
                Debug.AddLine(wxString::Format(
                    "SOCKSVR: Unknown command char received from client: %d\n",
                    (int)c));
                rval = 1;
                break;
        }
    } catch (const wxString &Msg) {
        POSSIBLY_UNUSED(Msg);
    }

    Debug.Write(
        wxString::Format("Sending socket response %d (0x%x)\n", rval, rval));

    // Send response
    sock->Write(&rval, 1);

    // Enable input events again.
    sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
}

void MyFrame::OnSockServerClientEvent(wxSocketEvent &event) {
    try {
        wxSocketBase *sock = event.GetSocket();

        // Make sure that SocketServer is not NULL
        if (!SocketServer) {
            throw ERROR_INFO("socket command when SocketServer == NULL");
        }

        // Now we process the event
        switch (event.GetSocketEvent()) {
            // Handle input from the client
            case wxSOCKET_INPUT:
                HandleSockServerInput(sock);
                break;

            // Handle disconnection of the client
            case wxSOCKET_LOST:
                Debug.AddLine("SOCKSVR: Client disconnected, deleting socket");

                // Remove the socket from the list of clients
                size_t n;
                n = s_clients.erase(sock);
                assert(n > 0);

                // Destroy the socket
                sock->Destroy();
                break;

            // Ignore other events
            default:
                break;
        }
    } catch (const wxString &Msg) {
        // Catch any exceptions and ignore them
        POSSIBLY_UNUSED(Msg);
    }

    // Add debug output to show that the event was processed
    Debug.AddLine("SOCKSVR: Client event processed");
}
