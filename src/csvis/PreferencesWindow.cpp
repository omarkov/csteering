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

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/ComboBox.h"
#include "glgooey/Slider.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "PreferencesWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

PreferencesWindow* g_PreferencesWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

PreferencesWindow::PreferencesWindow()
{
    g_PreferencesWindow = this;

    this->setSize(Vector2(430, 210));
    this->setText("Preferences");

    // Create objects 
    m_cursorStyleText = new StaticText(NULL, "Cursor Style:");
    m_cursorStyleCombo = new ComboBox(NULL, Rectangle(10, 10, 20, 20));
    m_cursorStyleCombo->addString("arrow");
    m_cursorStyleCombo->addString("normal cross");
    m_cursorStyleCombo->addString("diagonal cross");
    m_cursorStyleCombo->addString("crossing lines");
    m_cursorSizeText = new StaticText(NULL, "Cursor Size:");
    m_cursorSizeSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_cursorSizeSlider->setMaximum(500);
    m_cursorSizeSlider->setMinimum(50);
    m_cancelButton = new Gooey::Button(NULL, "Cancel");
    m_okButton = new Gooey::Button(NULL, "OK");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_cursorStyleCombo->selectionChanged.connect(this, &PreferencesWindow::cursorStyleComboChanged);
    m_cursorSizeSlider->valueChanged.connect(this, &PreferencesWindow::cursorSizeSliderChanged);
    m_cancelButton->pressed.connect(this, &PreferencesWindow::cancelButtonPressed);
    m_okButton->pressed.connect(this, &PreferencesWindow::okButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(170.0f);
    widths.push_back(40.0f);
    widths.push_back(0.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(10.0f); // space
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(0.0f); // space
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cursorStyleText, info);
    info.columnIndex = 1; info.rowIndex = 1; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cursorStyleCombo, info);
    info.columnIndex = 0; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cursorSizeText, info);
    info.columnIndex = 1; info.rowIndex = 2; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cursorSizeSlider, info);
    info.columnIndex = 0; info.rowIndex = 4; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 2; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_okButton, info);
}

PreferencesWindow::~PreferencesWindow()
{
    g_PreferencesWindow = NULL;

    delete m_okButton;
    delete m_cancelButton;
    delete m_cursorSizeSlider;
    delete m_cursorSizeText;
    delete m_cursorStyleCombo;
    delete m_cursorStyleText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void PreferencesWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void PreferencesWindow::showWindow(Vector2 position)
{
    if (!(this->isHidden()))
        return;

    // store current settings
    m_cursorStyle = g_GUIController->cursor()->style();
    m_cursorSize = g_GUIController->cursor()->size();
    
    // set Controlls
    m_cursorStyleCombo->selectStringAt(m_cursorStyle);
    m_cursorSizeSlider->setValue(m_cursorSize);
    
    StandardWindow::showWindow(position);
}

void PreferencesWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void PreferencesWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_okButton->enable();
    m_cancelButton->enable();
    m_cursorSizeSlider->enable();
    m_cursorSizeText->enable();
    m_cursorStyleCombo->enable();
    m_cursorStyleText->enable();

    StandardWindow::enableWindow();
}

void PreferencesWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_okButton->disable();
    m_cancelButton->disable();
    m_cursorSizeSlider->disable();
    m_cursorSizeText->disable();
    m_cursorStyleCombo->disable();
    m_cursorStyleText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void PreferencesWindow::cursorStyleComboChanged(const std::string& itemText)
{
    g_GUIController->cursor()->setStyle(m_cursorStyleCombo->selectedIndex());
}

void PreferencesWindow::cursorSizeSliderChanged(int value)
{
    g_GUIController->cursor()->setSize(value);
}

void PreferencesWindow::cancelButtonPressed()
{
    g_GUIController->cursor()->setStyle(m_cursorStyle);
    g_GUIController->cursor()->setSize(m_cursorSize);
    this->hideWindow();
}

void PreferencesWindow::okButtonPressed()
{
    this->hideWindow();
}
