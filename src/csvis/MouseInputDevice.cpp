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

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/GL>

#include <math.h>

#include "Application.h"
#include "GUIController.h"
#include "InputManager.h"
#include "MouseInputDevice.h"

// ----------------------------------------------------------------------------
// Our event handler for the mouse
// ----------------------------------------------------------------------------

class MouseDeviceHandler : public osgGA::GUIEventHandler
{
    MouseInputDevice* m_device;
    
protected:
    MouseDeviceHandler& operator= (const MouseDeviceHandler&)
    {
	return *this;
    }

public:
    MouseDeviceHandler(MouseInputDevice* device)
      :osgGA::GUIEventHandler()
    {
        m_device = device;
    }

    virtual bool handle(const osgGA::GUIEventAdapter& eventAdapter,
                        osgGA::GUIActionAdapter& actionAdapter)
    {
	switch(eventAdapter.getEventType()) {
	    // the mouse has been moved
            case osgGA::GUIEventAdapter::MOVE:
            case osgGA::GUIEventAdapter::DRAG:
            {
                unsigned int width, height;
                g_GUIController->screenSize(width, height);
                float x = width * (0.5f + eventAdapter.getX() / 2.0f);
                float y = height * (0.5f - eventAdapter.getY() / 2.0f);

                return g_InputManager->handlePointerMove(x, y);
            }

	    // a mouse button has been pressed
            case osgGA::GUIEventAdapter::PUSH:
                switch(eventAdapter.getButton())
                {
                    case(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON):
                        m_device->m_leftButtonDown = true;
                        break;
                    case(osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON):
                        m_device->m_middleButtonDown = true;
                        break;
                    case(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON):
                        m_device->m_rightButtonDown = true;
                        break;
                }
                return g_InputManager->handlePointerButtonDown(eventAdapter.getButton());

	    // a mouse button has been released
            case osgGA::GUIEventAdapter::RELEASE:
                switch(eventAdapter.getButton())
                {
                    case(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON):
                        m_device->m_leftButtonDown = false;
                        break;
                    case(osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON):
                        m_device->m_middleButtonDown = false;
                        break;
                    case(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON):
                        m_device->m_rightButtonDown = false;
                        break;
                }
                return g_InputManager->handlePointerButtonUp(eventAdapter.getButton());

            default:
                return false;
	}
    }

    virtual void accept(const osgGA::GUIEventHandlerVisitor& visitor)
    {
    }
};

// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

MouseInputDevice::MouseInputDevice(const std::string& name)
    : InputDevice(name), m_leftButtonDown(false),  m_middleButtonDown(false), m_rightButtonDown(false)
{
    g_Application->viewer()->getEventHandlerList().push_front(new MouseDeviceHandler(this));
}

// ----------------------------------------------------------------------------
// Represent on OSD
// ----------------------------------------------------------------------------

void MouseInputDevice::draw()
{
    glLineWidth(1.0f);
    
    // pressed buttons
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    if (m_leftButtonDown)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-30, -35);
        glVertex2i(-30, -50);
        glVertex2i(-20, -50);
        glVertex2i(-20, -35);
        glEnd();
    }
    if (m_middleButtonDown)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-20, -35);
        glVertex2i(-20, -50);
        glVertex2i(-10, -50);
        glVertex2i(-10, -35);
        glEnd();
    }
    if (m_rightButtonDown)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-10, -35);
        glVertex2i(-10, -50);
        glVertex2i(0, -50);
        glVertex2i(0, -35);
        glEnd();
    }
    
    // mouse
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i <= 180; i++)
        glVertex2f(-15 + cos(i*3.14159/180.0f)*15, -15 + sin(i*3.14159/180.0f)*15);
    glVertex2i(-30, -50);
    glVertex2i(0, -50);
    glEnd();
    glBegin(GL_LINES);
    glVertex2i(-30, -35);
    glVertex2i(0, -35);
    glVertex2i(-20, -50);
    glVertex2i(-20, -35);
    glVertex2i(-10, -50);
    glVertex2i(-10, -35);
    glEnd();
    
    // lines and text
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    switch (g_GUIController->state())
    {
        case (DEFAULT_MODE):
        case (EDIT_MODE):
            glBegin(GL_LINE_STRIP);
            glVertex2i(-25, -45);
            glVertex2i(-35, -55);
            glVertex2i(-45, -55);
            glEnd();
            g_GUIController->osd()->drawInputDeviceText("Select", -50, -55);
            if (!g_GUIController->visibleWindows())
            {
                glBegin(GL_LINE_STRIP);
                glVertex2i(-5, -40);
                glVertex2i(-20, -25);
                glVertex2i(-45, -25);
                glEnd();
                if (g_GUIController->activeMenu())
                    g_GUIController->osd()->drawInputDeviceText("Close Menu", -50, -25);
                else
                    g_GUIController->osd()->drawInputDeviceText("Open Menu", -50, -25);
            }
            break;
        case (CAMERA_MODE):
            glBegin(GL_LINE_STRIP);
            glVertex2i(-25, -45);
            glVertex2i(-35, -55);
            glVertex2i(-45, -55);
            glEnd();
            g_GUIController->osd()->drawInputDeviceText("Rotate", -50, -55);
            glBegin(GL_LINE_STRIP);
            glVertex2i(-15, -40);
            glVertex2i(-45, -40);
            glEnd();
            g_GUIController->osd()->drawInputDeviceText("Move", -50, -40);
            glBegin(GL_LINE_STRIP);
            glVertex2i(-5, -40);
            glVertex2i(-20, -25);
            glVertex2i(-45, -25);
            glEnd();
            g_GUIController->osd()->drawInputDeviceText("Zoom", -50, -25);
            break;
    }
}
