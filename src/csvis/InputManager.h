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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <vector>
#include <list>
#include <string>

#include "InputDevice.h"
#include "InputNotificationMixin.h"


bool isShiftPressed(void);
bool isAltPressed(void);
bool isCtrlPressed(void);

class InputManager
{
    std::vector<InputDevice*> m_inputDevices;
    InputDevice *m_primaryInputDevice;

    std::list<InputNotificationMixin*> m_objectsToNotify;

    friend class EventHandler;

    void updateDevices();

public:
    InputManager();
    ~InputManager();

    bool init();

    // add an input device
    void addInputDevice(InputDevice* device);
    // get an input device by name
    InputDevice* inputDevice(const std::string& name);

    void setPrimaryInputDevice(const std::string& name);
    InputDevice* primaryInputDevice() { return m_primaryInputDevice; }

    void enableInputDevice(const std::string& name);
    void disableInputDevice(const std::string& name);

    void addInputListener(InputNotificationMixin* object);
    void removeInputListener(InputNotificationMixin* object);

    // handler functions
    bool handleKeyDown(int key);
    bool handleKeyUp(int key);
    bool handlePointerMove(float x, float y);
    bool handlePointerButtonDown(int button);
    bool handlePointerButtonUp(int button);
};

extern InputManager* g_InputManager;

#endif // INPUTMANAGER_H
