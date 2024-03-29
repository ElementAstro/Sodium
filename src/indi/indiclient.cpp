/*
 *  phdindiclient.cpp
 *  LGuider2 Guiding
 *
 *  Created by Andy Galasso
 *  Copyright (c) 2018 Andy Galasso
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
 *    Neither the name of Open LGuider Guiding, openphdguiding.org, nor the names of its
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

#include "indiclient.h"

LightIndiClient::LightIndiClient()
    :
    m_disconnecting(false)
{
}

LightIndiClient::~LightIndiClient()
{
}

void LightIndiClient::serverDisconnected(int exit_code)
{
    m_disconnecting = true;
    IndiServerDisconnected(exit_code);
    m_disconnecting = false;
}

void LightIndiClient::serverConnected()
{
    // nothing to do yet
    // for INDI Core 1.9.9, 2.0.0, the function is called before requesting to retrieve device information.
    // If the function implementation waits for information, a deadlock occurs.
    //
    // see LightIndiClient::connectServer override function below
}

bool LightIndiClient::connectServer()
{
    // Call the original function.
    bool ok = INDI::BaseClient::connectServer();

    // After the original function was completed,
    // the device information request was made in the INDI Core library.

    // If connected, inform via the IndiServerConnected function,
    // which replaces the serverConnected function.
    if (ok)
    {
        IndiServerConnected();
    }

    return ok;
}

bool LightIndiClient::DisconnectIndiServer()
{
    // suppress any attempt to call disconnectServer from the
    // serverDisconnected callback.  Some serverDisconnected callbacks
    // in LGuider call disconnectServer which causes a crash (unhandled
    // exception) if the callbacks are invoked in the INDI listener
    // thread since disconnectServer will try to join the listener
    // thread, throwing a C++ runtime exception (deadlock)

    if (m_disconnecting)
    {
        return true;
    }

    return disconnectServer();
}
