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
#include "glgooey/ListBox.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "AboutWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

AboutWindow* g_AboutWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

AboutWindow::AboutWindow()
{
    g_AboutWindow = this;

    this->setSize(Vector2(650, 400));
    this->setText("About");

    // Create objects 
    m_nameText = new StaticText(NULL, "STORM v1.0");
    m_textBox = new ListBox(NULL, Rectangle(10, 10, 20, 20));
    m_textBox->addString("Author:");
    m_textBox->addString("  Studienprojekt A - Computational Steering");
    m_textBox->addString("  University of Stuttgart");
    m_textBox->addString("");
    m_textBox->addString("Homepage:");
    m_textBox->addString("  http://duck.informatik.uni-stuttgart.de");
    m_textBox->addString("  /projects/csteering");
    m_textBox->addString("");
    m_textBox->addString("The Team:");
    m_textBox->addString("  Ruediger Hohloch");
    m_textBox->addString("  Benjamin Jung");
    m_textBox->addString("  Jens Nausedat");
    m_textBox->addString("  Oliver Markovic");
    m_textBox->addString("  Mikhail Prokharau");
    m_textBox->addString("  Tim Rathgeber");
    m_textBox->addString("  Dominik Roessler");
    m_textBox->addString("  Clemens Spenrath");
    m_textBox->addString("  Tobias Walter");
    m_closeButton = new Button(NULL, "Close");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_closeButton->pressed.connect(this, &AboutWindow::closeButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(0.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(10.0f); // space
    heights.push_back(40.0f);
    heights.push_back(0.0f);
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_nameText, info);
    info.columnIndex = 0; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_textBox, info);
    info.columnIndex = 0; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_closeButton, info);
}

AboutWindow::~AboutWindow()
{
    g_AboutWindow = NULL;

    delete m_closeButton;
    delete m_textBox;
    delete m_nameText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void AboutWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void AboutWindow::showWindow(Vector2 position)
{
    if (!(this->isHidden()))
        return;

    StandardWindow::showWindow(position);
}

void AboutWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();
}

void AboutWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_closeButton->enable();
    m_textBox->enable();
    m_nameText->enable();

    StandardWindow::enableWindow();
}

void AboutWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_closeButton->disable();
    m_textBox->disable();
    m_nameText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void AboutWindow::closeButtonPressed()
{
    this->hideWindow();
}
