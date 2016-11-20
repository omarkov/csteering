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

#include <fstream>

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/ListControl.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "HelpWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

HelpWindow* g_HelpWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

HelpWindow::HelpWindow()
{
    g_HelpWindow = this;

    this->setSize(Vector2(600, 450));
    this->setText("Shortcuts");

    // Create objects 
    m_textBox = new ListControl(NULL, Rectangle(10, 10, 20, 20), 2);
    std::vector<float> colWidths;
    colWidths.push_back(80.0f);
    colWidths.push_back(0.0f);
    m_textBox->setColumnWidths(colWidths);
    std::vector<std::string> row;
    row.push_back("Space"); row.push_back("Toggle Camera Mode");
    m_textBox->addRow(row); row.clear();
    row.push_back("Ctrl"); row.push_back("Move in XZ-plane");
    m_textBox->addRow(row); row.clear();
    row.push_back("Shift"); row.push_back("Move in XZ-plane");
    m_textBox->addRow(row); row.clear();
    row.push_back("Alt"); row.push_back("Move in YZ-plane");
    m_textBox->addRow(row); row.clear();
    row.push_back("W"); row.push_back("Toggle Wireframe");
    m_textBox->addRow(row); row.clear();
    row.push_back("L"); row.push_back("Toggle Light");
    m_textBox->addRow(row); row.clear();
    row.push_back("R"); row.push_back("Reset Camera");
    m_textBox->addRow(row); row.clear();
    m_closeButton = new Button(NULL, "Close");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_closeButton->pressed.connect(this, &HelpWindow::closeButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(0.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(0.0f);
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 0; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_textBox, info);
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_closeButton, info);
}

HelpWindow::~HelpWindow()
{
    g_HelpWindow = NULL;

    delete m_closeButton;
    delete m_textBox;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void HelpWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void HelpWindow::showWindow(Vector2 position)
{
    if (!(this->isHidden()))
        return;

    StandardWindow::showWindow(position);
}

void HelpWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();
}

void HelpWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_closeButton->enable();
    m_textBox->enable();

    StandardWindow::enableWindow();
}

void HelpWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_closeButton->disable();
    m_textBox->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void HelpWindow::closeButtonPressed()
{
    this->hideWindow();
}
