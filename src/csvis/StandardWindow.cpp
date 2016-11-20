// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Clemens Spenrath <clemens-mail@gmx.de> 
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

#include "GUIController.h"

#include "glgooey/FrameWindow.h"
#include "glgooey/Panel.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"
#include "glgooey/EditField.h"

#include "StandardWindow.h"
#include "MessageWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

StandardWindow::StandardWindow()
: FrameWindow(Rectangle(0, 0, 100, 100), 0, ""), m_isHidden(true), m_isDisabled(false)
{
    // try to get the windowmanager
    if ((m_WindowManager = &WindowManager::instance()) == NULL)
        return;

    // client panel: size doesnt matter, it will be alligned to the FrameWindow
    m_panel = new Panel(Rectangle(10, 10, 20, 20), this);

    // make the window movable
    this->enableMovement();

    // put the panel in the frame window
    this->setClientWindow(m_panel);

    this->hide();
    m_WindowManager->addWindow(this);
}

StandardWindow::~StandardWindow()
{
    this->hideWindow();
    this->setClientWindow(NULL);

    delete m_panel;
}

Gooey::Core::Vector2 StandardWindow::getScreenCenter()
{
    Gooey::Core::Vector2 size = m_WindowManager->applicationSize();
    return Vector2(size.x()/2.0f, size.y()/2.0f);
}

void StandardWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void StandardWindow::showWindow(Vector2 position)
{
    Vector2 size = this->size();
    this->setPosition(Vector2(position.x()-(size.x()/2.0f), position.y()-(size.y()/2.0f)));
    this->show();
    m_WindowManager->setFocusedWindow(this);
    this->setAsActiveWindow();
    m_isHidden = false;
    g_GUIController->setVisibleWindows(g_GUIController->visibleWindows() + 1);
}

void StandardWindow::hideWindow()
{
    this->hide();
    m_isHidden = true;
    g_GUIController->setVisibleWindows(g_GUIController->visibleWindows() - 1);
}

void StandardWindow::enableWindow()
{
    this->enable();
    m_isDisabled = false;
}

void StandardWindow::disableWindow()
{
    this->disable();
    m_isDisabled = true;
}

// ---------------------------------------------------------------------------
// helper functions
// ---------------------------------------------------------------------------

bool StandardWindow::getValueFromEdit(Gooey::EditField* edit, double& value, double min, double max, const std::string& errorMessage)
{
    double val = atof(edit->text().c_str());
    // this may be too forgiving
    if ((val == 0) && (edit->text().find("0") == std::string::npos))
        {
        if (errorMessage != "")
            g_MessageWindow->showWindow(MESSAGETYPE_ERROR, errorMessage);
            return false;
        }
    if (((min != -1.0f) && (val < min)) || ((max != -1.0f) && (val > max)))
        {
        if (errorMessage != "")
            g_MessageWindow->showWindow(MESSAGETYPE_ERROR, errorMessage);
            return false;
        }
    value = val;
    return true;
}

bool StandardWindow::getValueFromEdit(Gooey::EditField* edit, int& value, int min, int max, const std::string& errorMessage)
{
    int val = atoi(edit->text().c_str());
    // this may be too forgiving
    if ((val == 0) && (edit->text().find("0") == std::string::npos))
        {
        if (errorMessage != "")
            g_MessageWindow->showWindow(MESSAGETYPE_ERROR, errorMessage);
            return false;
        }
    if (((min != -1) && (val < min)) || ((max != -1) && (val > max)))
        {
        if (errorMessage != "")
            g_MessageWindow->showWindow(MESSAGETYPE_ERROR, errorMessage);
            return false;
        }
    value = val;
    return true;
}
