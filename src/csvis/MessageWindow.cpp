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

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/Panel.h"
#include "glgooey/Button.h"
#include "glgooey/StaticText.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "MessageWindow.h"
#include "GUIController.h"

using namespace Gooey;
using namespace Gooey::Core;

MessageWindow* g_MessageWindow = NULL;


// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

MessageWindow::MessageWindow()
{

    g_MessageWindow = this;

    this->setSize(Vector2(600, 200));

    // Create objects
    m_messageText = new StaticText(NULL, "Message");
    m_okButton = new Gooey::Button(NULL, "OK");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_okButton->pressed.connect(this, &MessageWindow::okButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(200.0f);
    widths.push_back(0.0f);
    widths.push_back(200.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(20.0f); // space
    heights.push_back(0.0f);
    heights.push_back(20.0f); // space
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 3; info.verticalSpan = 1;
    m_panel->addChildWindow(m_messageText, info);
    info.columnIndex = 1; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_okButton, info);
}

MessageWindow::~MessageWindow()
{
    g_MessageWindow = NULL;

    delete m_okButton;
    delete m_messageText;
}

void MessageWindow::showWindow()
{
    return;
}

void MessageWindow::showWindow(Vector2 position)
{
    return;
}

void MessageWindow::showWindow(MessageType type, const std::string& message)
{
    this->showWindow(getScreenCenter(), type, message);
}

void MessageWindow::showWindow(Vector2 position, MessageType type, const std::string& message)
{
    if (!(this->isHidden()))
        return;

    switch (type) {
        case MESSAGETYPE_INFORMATION:
            this->setText("Information");
            break;

        case MESSAGETYPE_WARNING:
            this->setText("Warning");
            break;

        case MESSAGETYPE_ERROR:
            this->setText("Error");
            break;

        default:
            return;
    }
    m_messageText->setText(message);

    g_GUIController->disableAllWindows(this);
    StandardWindow::showWindow(position);
}

void MessageWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    g_GUIController->enableAllWindows();
}

void MessageWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_okButton->enable();
    m_messageText->enable();
    
    StandardWindow::enableWindow();
}

void MessageWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_okButton->disable();
    m_messageText->disable();
}

//////////////////////////////////////////////////////////
// Events connected to the Gooey-Items
//////////////////////////////////////////////////////////

void MessageWindow::okButtonPressed()
{
    this->hideWindow();
}
