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

#include <osg/GL>

#include "Application.h"
#include "Menu.h"
#include "MenuItem.h"
#include "GUIController.h"
#include "MenuEventHandler.h"
#include "glgooey/sigslot.h"

#include "glgooey/core/Rectangle.h"
#include "glgooey/core/Vector2.h"
#include "glgooey/Panel.h"
#include "glgooey/StaticText.h"

const double PI=3.14159265f;
const double PULSE_SPEED = 12.0f;

const unsigned int BUTTON_WIDTH = 200;
const unsigned int BUTTON_HEIGHT = 40;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

MenuItem::MenuItem()
{
    MenuItem("", NULL, NULL);
}

MenuItem::MenuItem(const std::string& text, MenuEventHandler* ctrl, void (MenuEventHandler::*fkt)())
: m_parent(NULL), m_isMenuItemEnabled(true), m_pulse(0.0f)
{
    m_font = new Gooey::Font::TypeFace(g_Application->dataPath("fonts/andalemo.ttf"), 12, 100);
    m_text = text;
    // wire up the function
    if ((ctrl != NULL) && (fkt != NULL))
        this->m_pressed.connect(ctrl, fkt);
}

MenuItem::~MenuItem()
{
    // FIXME: caching issues
    //delete m_font;
}

void MenuItem::setText(const std::string& menuItemText)
{
    m_text = menuItemText;
}

void MenuItem::setPosition(const Gooey::Core::Vector2 position)
{
    m_position = position;
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------


void MenuItem::performAction()
{
    this->parent()->hideMenu();
    this->m_pressed();
}

void MenuItem::drawMenuItem(bool highlighted, double elapsed)
{
    m_pulse += (elapsed*PULSE_SPEED/1000.0f);
    if (m_pulse >= 2.0f*PI)
        m_pulse = 0;
    float pulse = 0.5f + sin(m_pulse)/2.0f;

    if (this->isEnabled())
        {
        if (highlighted)
            {
            glBegin(GL_QUADS);
            glColor4f(0.2f, 0.2f, 1.0f, 0.5f + pulse/2.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f-10.0f*pulse, m_position.y()-BUTTON_HEIGHT/2.0f-5.0f*pulse);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f+10.0f*pulse, m_position.y()-BUTTON_HEIGHT/2.0f-5.0f*pulse);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f+10.0f*pulse, m_position.y()+BUTTON_HEIGHT/2.0f+5.0f*pulse);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f-10.0f*pulse, m_position.y()+BUTTON_HEIGHT/2.0f+5.0f*pulse);
            glEnd();
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f-10.0f*pulse, m_position.y()-BUTTON_HEIGHT/2.0f-5.0f*pulse);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f+10.0f*pulse, m_position.y()-BUTTON_HEIGHT/2.0f-5.0f*pulse);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f+10.0f*pulse, m_position.y()+BUTTON_HEIGHT/2.0f+5.0f*pulse);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f-10.0f*pulse, m_position.y()+BUTTON_HEIGHT/2.0f+5.0f*pulse);
            glEnd();
            } else {
            glBegin(GL_QUADS);
            glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glEnd();
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glEnd();
            }
        } else {
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()-BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()+BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glVertex2f(m_position.x()-BUTTON_WIDTH/2.0f, m_position.y()+BUTTON_HEIGHT/2.0f);
            glEnd();
        }
    if (this->isEnabled())
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    else
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    m_font->render(m_text, Gooey::Core::Vector2(m_position.x()-(m_font->width(m_text)/2.0f),
                                                m_position.y()+m_font->descent()));
}
