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

#include <assert.h>

#include "GUIController.h"
#include "OnscreenDisplay.h"

#include "ProbeManager.h"
#include "MouseInputDevice.h"
#include "DTrackInputDevice.h"
#include "InputManager.h"

#include "FANClasses.h"

InputManager* g_InputManager = 0;


// ----------------------------------------------------------------------------
// Functions used by GLGooey
// ----------------------------------------------------------------------------

static bool shiftDown = false;
static bool altDown = false;
static bool ctrlDown = false;

bool isShiftPressed() { return shiftDown; }
bool isAltPressed() { return altDown; }
bool isCtrlPressed() { return ctrlDown; }

void modifierKeyChanged(int key, bool state)
{
   switch (key) {
        case osgGA::GUIEventAdapter::KEY_Shift_L:
        case osgGA::GUIEventAdapter::KEY_Shift_R:
            shiftDown = state;
            break;
            
        case osgGA::GUIEventAdapter::KEY_Control_L:
        case osgGA::GUIEventAdapter::KEY_Control_R:
            ctrlDown = state;
            break;
            
        case osgGA::GUIEventAdapter::KEY_Alt_L:
        case osgGA::GUIEventAdapter::KEY_Alt_R:
            altDown = state;
            break;
    }
}

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

InputManager::InputManager()
    : m_primaryInputDevice(0)
{
}

InputManager::~InputManager()
{
    std::vector<InputDevice*>::const_iterator it;

    for (it = m_inputDevices.begin(); it != m_inputDevices.end(); ++it)
        delete (*it);
}


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

bool InputManager::init()
{
    // add a mouse input device
    addInputDevice(new MouseInputDevice("Mouse"));
    enableInputDevice("Mouse");
    setPrimaryInputDevice("Mouse");

    if(FAN::app->config->checkKey("visinputdevice", "dtrack"))
    {
        DTrackInputDevice* tracker = new DTrackInputDevice("Tracker");

        if (!tracker->connect(FAN::app->config->getValue("dtrackhost", "129.69.215.28"), atoi(FAN::app->config->getValue("dtrackport", "5001"))))
            return false;

        tracker->setEnabled(true);
        tracker->setSensitivityFactor(strtod(FAN::app->config->getValue("dtracksensitivity", "0.8"), NULL));
        tracker->setyOffset(strtod(FAN::app->config->getValue("dtrackyoffset", "1200.0"), NULL));
        addInputDevice(tracker);
        setPrimaryInputDevice("Tracker");
        disableInputDevice("Mouse");
    }

    return true;
}


// ----------------------------------------------------------------------------
// Handler functions called by the EventHandler
// ----------------------------------------------------------------------------

bool InputManager::handleKeyDown(int key)
{
    // handle Shift, Ctrl & Alt for glgooey
    modifierKeyChanged(key, true);

    // pass the keypress to glgooey
    bool handled = g_GUIController->windowManager()->onChar(key);
    // do nothing else if windows are visible
    if (g_GUIController->visibleWindows()) return true;

    // notify all listeners    
    std::list<InputNotificationMixin*>::const_iterator it;
    for (it = m_objectsToNotify.begin(); it != m_objectsToNotify.end(); ++it)
        (*it)->handleButtonPressed(key);

    return handled;
}

bool InputManager::handleKeyUp(int key)
{
    // handle Shift, Ctrl & Alt for glgooey
    modifierKeyChanged(key, false);

    // do nothing else if windows are visible
    if (g_GUIController->visibleWindows()) return true;

    // notify all listeners
    std::list<InputNotificationMixin*>::const_iterator it;
    for (it = m_objectsToNotify.begin(); it != m_objectsToNotify.end(); ++it)
        (*it)->handleButtonReleased(key);

    return false;
}

bool InputManager::handlePointerMove(float x, float y)
{
    // pass the event to glgooey
    g_GUIController->windowManager()->onMouseMove((unsigned int)x, (unsigned int)y);

    // update the cursor
    g_GUIController->cursor()->setPosition(x, y);

    // do nothing else if windows are visible
    if (g_GUIController->visibleWindows()) return true;
    
    // update the menu if active
    if (g_GUIController->activeMenu())
        g_GUIController->activeMenu()->performHighlight(Gooey::Core::Vector2(x, y));

    // notify all listeners
    std::list<InputNotificationMixin*>::const_iterator it;
    for (it = m_objectsToNotify.begin(); it != m_objectsToNotify.end(); ++it)
        (*it)->handleMove(x, y);

    return false;
}

bool InputManager::handlePointerButtonDown(int button)
{
    bool handled = false;
    float fx, fy;

    g_GUIController->cursor()->getPosition(fx, fy);

    unsigned int cursorPosX = (unsigned int)fx;
    unsigned int cursorPosY = (unsigned int)fy;

    // menu
    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
        // left button
        if (g_GUIController->activeMenu()) {
            // just do nothing, menuAction happens on PointerUp to avoid glgooey catch the Button-Up-Event
            return true;
        }
    } else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) {
    } else {
        // right button
        // activate or de-activate the menu
        if (g_GUIController->state() != CAMERA_MODE)
        {
            if (g_GUIController->activeMenu()) {
                g_GUIController->activeMenu()->hideMenu();
            } else {
                if (!g_GUIController->visibleWindows()
                    && (g_GUIController->state() != CAMERA_MODE))
                {
                    if (g_ProbeManager->selectedProbe())
                        g_GUIController->getMenu("ROOT_PROBE")
                          ->showMenu(Gooey::Core::Vector2(cursorPosX, cursorPosY));
                    else
                        g_GUIController->getMenu("ROOT_MAIN")
                          ->showMenu(Gooey::Core::Vector2(cursorPosX, cursorPosY));
                }
            }
            return true;
        }
    }
    
    handled = g_GUIController->visibleWindows();

    // pass the event to glgooey
    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
        // left button
        g_GUIController->windowManager()->onLeftButtonDown(cursorPosX, cursorPosY);
    } else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) {
        g_GUIController->windowManager()->onMiddleButtonDown(cursorPosX, cursorPosY);
    } else {
        // right button
        g_GUIController->windowManager()->onRightButtonDown(cursorPosX, cursorPosY);        
    }

    // do nothing else if windows are/were visible
    if (handled) return true;

    if (g_GUIController->osd()->handleClick(cursorPosX, cursorPosY))
        return true;

    // handle pick
    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
        // left button
        handled = g_GUIController->handlePick();
    } else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) {
        // middle button
    } else {
        // right button
    }

    // notify all listeners
    std::list<InputNotificationMixin*>::const_iterator it;
    for (it = m_objectsToNotify.begin(); it != m_objectsToNotify.end(); ++it)
        (*it)->handleButtonPressed(button);

    return handled;
}

bool InputManager::handlePointerButtonUp(int button)
{
    // glgooey wants cursor coordinates as integers
    float fx, fy;
    unsigned int cursorPosX, cursorPosY;
    g_GUIController->cursor()->getPosition(fx, fy);

    cursorPosX = (unsigned int)fx;
    cursorPosY = (unsigned int)fy;

    bool handled = (g_GUIController->activeMenu());

    // menuAction
    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
        if (g_GUIController->activeMenu()) {
            g_GUIController->activeMenu()->performMenuAction();
            return true;
        }
    }

    // do nothing else if a menu is/was visible
    if (handled) return true;

    handled = g_GUIController->visibleWindows();

    // pass the event to glgooey
    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        g_GUIController->windowManager()->onLeftButtonUp(cursorPosX, cursorPosY);
    else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
        g_GUIController->windowManager()->onMiddleButtonUp(cursorPosX, cursorPosY);
    else
        g_GUIController->windowManager()->onRightButtonUp(cursorPosX, cursorPosY);

    // do nothing else if windows are visible
    if (handled) return true;

    // notify all listeners
    std::list<InputNotificationMixin*>::const_iterator it;
    for (it = m_objectsToNotify.begin(); it != m_objectsToNotify.end(); ++it)
        (*it)->handleButtonReleased(button);
        
    return false;
}



// ----------------------------------------------------------------------------
// Input device management
// ----------------------------------------------------------------------------

void InputManager::addInputDevice(InputDevice* device)
{
    assert(device);

    // i don't care about duplicates really
    m_inputDevices.push_back(device);
}

InputDevice* InputManager::inputDevice(const std::string& name)
{
    std::vector<InputDevice*>::const_iterator it;

    for (it = m_inputDevices.begin(); it != m_inputDevices.end(); ++it)
        if ((*it)->deviceName() == name)
            return (*it);

    return NULL;
}

void InputManager::setPrimaryInputDevice(const std::string& name)
{
    m_primaryInputDevice = inputDevice(name);
}

void InputManager::enableInputDevice(const std::string& name)
{
    InputDevice *device = inputDevice(name);

    if (device)
        device->setEnabled(true);
}

void InputManager::disableInputDevice(const std::string& name)
{
    InputDevice *device = inputDevice(name);

    if (device)
        device->setEnabled(false);
}

void InputManager::updateDevices()
{
    std::vector<InputDevice*>::const_iterator it;

    for (it = m_inputDevices.begin(); it != m_inputDevices.end(); ++it)
        if ((*it)->isEnabled())
            (*it)->update();
}


// ----------------------------------------------------------------------------
// Input listener support
// ----------------------------------------------------------------------------

void InputManager::addInputListener(InputNotificationMixin* object)
{
    assert(object);

    m_objectsToNotify.push_back(object);
}

void InputManager::removeInputListener(InputNotificationMixin* object)
{
    assert(object);

    m_objectsToNotify.remove(object);
}
