// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org> 
//   All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  o Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer. 
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution. 
//  o Neither the name of the author nor the names of the contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef DTRACKINPUTDEVICE_H
#define DTRACKINPUTDEVICE_H

#include <netinet/in.h>
#include "InputDevice.h"

enum FlyStickButton {
    FLY_NO_BUTTON = 0,
    FLY_FRONT_BUTTON  = 1,
    FLY_LEFT_BUTTON   = 8,
    FLY_MIDDLE_BUTTON = 4,
    FLY_RIGHT_BUTTON  = 2,
    FLY_UP      = 64,
    FLY_LEFT    = 32,
    FLY_DOWN    = 16,
    FLY_RIGHT   = 128
};

class DTrackInputDevice : public InputDevice
{
    //static const int localPort = 22301;
    static const int localAddress = INADDR_ANY;
    static const int packetLength = 1024;

    std::string m_remoteHost;
    int m_localPort;
    int m_remotePort;
    int m_inputSocket;
    int m_outputSocket;

    struct {
      unsigned int id;
      float quality;
      unsigned int button;
      float location[3];
      float angle[3];
      float matrix[9];
    } m_trackerData;

    unsigned int m_oldButtonState;
  
    char m_trackerMessage[packetLength];

    bool processUDPStack();
    
public:
    DTrackInputDevice(const std::string& name);
    virtual ~DTrackInputDevice();

    bool connect(const std::string& host, int port);
    bool disconnect();

    bool isButtonPressed(FlyStickButton button) { return m_trackerData.button & button; }

    virtual void update();

    virtual bool provides3DData() { return true; };
    
    // draw device information
    virtual void draw();
};

#endif // DTRACKINPUTDEVICE_H
