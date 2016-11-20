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

#include "Application.h"
#include "TriangulatedModel.h"
#include "GUIController.h"
#include "simRemoteTypes.h"

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/EditField.h"
#include "glgooey/ComboBox.h"
#include "glgooey/Slider.h"
#include "glgooey/CheckBox.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "RibbonProbeWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

RibbonProbeWindow* g_RibbonProbeWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

RibbonProbeWindow::RibbonProbeWindow()
{
    g_RibbonProbeWindow = this;

    this->setSize(Vector2(600, 490));
    this->setText("Edit Ribbon Probe");

    // Create objects 
    m_probeHeightText = new StaticText(NULL, "Probe height:");
    m_probeHeightSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_probeHeightSlider->setMinimum(1);
    m_probeHeightEdit = new EditField(NULL, "");
    m_ribbonCountText = new StaticText(NULL, "Ribbon count:");
    m_ribbonCountSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_ribbonCountSlider->setMinimum(1);
    m_ribbonCountSlider->setMaximum(50);
    m_ribbonCountEdit = new EditField(NULL, "");
    m_ribbonWidthText = new StaticText(NULL, "Ribbon width %:");
    m_ribbonWidthSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_ribbonWidthSlider->setMinimum(0);
    m_ribbonWidthSlider->setMaximum(100);
    m_ribbonWidthEdit = new EditField(NULL, "");
    m_criteriaText = new StaticText(NULL, "Criteria:");
    m_criteriaCombo = new ComboBox(NULL, Rectangle(10, 10, 20, 20));
    m_criteriaCombo->addString("Density");
    m_criteriaCombo->addString("Pressure");
    m_criteriaCombo->addString("Velocity");
    m_criteriaText->hide(); // currently not supported
    m_criteriaCombo->hide(); // currently not supported
    m_colormapText = new StaticText(NULL, "Colormap:");
    m_colormapCombo = new ComboBox(NULL, Rectangle(10, 10, 20, 20));
    m_colormapCombo->addString("Temperature");
    m_colormapCombo->addString("Magenta");
    m_colormapCombo->addString("Spectral");
    m_colormapCombo->addString("Blue-Red");
    m_colormapCombo->addString("Blue-White");
    m_colormapCombo->addString("Green-Yellow-Red");
    m_colorRangeText = new StaticText(NULL, "Color-Range:");
    m_colorRangeCheck = new CheckBox(NULL, "Use custom Min/Max-Values", NULL);
    m_colorMinText = new StaticText(NULL, "Minimum:");
    m_colorMinSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_colorMinSlider->setMinimum(0);
    m_colorMinSlider->setMaximum(10000);
    m_colorMinEdit = new EditField(NULL, "");
    m_colorMaxText = new StaticText(NULL, "Maximum:");
    m_colorMaxSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_colorMaxSlider->setMinimum(0);
    m_colorMaxSlider->setMaximum(10000);
    m_colorMaxEdit = new EditField(NULL, "");
    m_cancelButton = new Button(NULL, "Cancel");
    m_okButton = new Button(NULL, "OK");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_probeHeightSlider->sliderMoved.connect(this, &RibbonProbeWindow::probeHeightSliderMoved);
    m_probeHeightEdit->returnPressed.connect(this, &RibbonProbeWindow::probeHeightEditReturnPressed);
    m_ribbonCountSlider->sliderMoved.connect(this, &RibbonProbeWindow::ribbonCountSliderMoved);
    m_ribbonCountEdit->returnPressed.connect(this, &RibbonProbeWindow::ribbonCountEditReturnPressed);
    m_ribbonWidthSlider->sliderMoved.connect(this, &RibbonProbeWindow::ribbonWidthSliderMoved);
    m_ribbonWidthEdit->returnPressed.connect(this, &RibbonProbeWindow::ribbonWidthEditReturnPressed);
    m_colorRangeCheck->clicked.connect(this, &RibbonProbeWindow::colorRangeCheckClicked);
    m_colorMinSlider->sliderMoved.connect(this, &RibbonProbeWindow::colorMinSliderMoved);
    m_colorMinEdit->returnPressed.connect(this, &RibbonProbeWindow::colorMinEditReturnPressed);
    m_colorMaxSlider->sliderMoved.connect(this, &RibbonProbeWindow::colorMaxSliderMoved);
    m_colorMaxEdit->returnPressed.connect(this, &RibbonProbeWindow::colorMaxEditReturnPressed);
    m_cancelButton->pressed.connect(this, &RibbonProbeWindow::cancelButtonPressed);
    m_okButton->pressed.connect(this, &RibbonProbeWindow::okButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(0.0f);
    widths.push_back(300.0f);
    widths.push_back(100.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(10.0f); // space
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(20.0f); // space
    heights.push_back(40.0f);
    heights.push_back(20.0f); // space
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    heights.push_back(10.0f); // space
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_probeHeightText, info);
    info.columnIndex = 1; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_probeHeightSlider, info);
    info.columnIndex = 2; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_probeHeightEdit, info);
    info.columnIndex = 0; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonCountText, info);
    info.columnIndex = 1; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonCountSlider, info);
    info.columnIndex = 2; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonCountEdit, info);
    info.columnIndex = 0; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonWidthText, info);
    info.columnIndex = 1; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonWidthSlider, info);
    info.columnIndex = 2; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ribbonWidthEdit, info);
    info.columnIndex = 0; info.rowIndex = 5; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_criteriaText, info);
    info.columnIndex = 1; info.rowIndex = 5; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_criteriaCombo, info);
    info.columnIndex = 0; info.rowIndex = 7; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colormapText, info);
    info.columnIndex = 1; info.rowIndex = 7; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colormapCombo, info);
    info.columnIndex = 0; info.rowIndex = 8; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorRangeText, info);
    info.columnIndex = 1; info.rowIndex = 8; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorRangeCheck, info);
    info.columnIndex = 0; info.rowIndex = 9; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMinText, info);
    info.columnIndex = 1; info.rowIndex = 9; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMinSlider, info);
    info.columnIndex = 2; info.rowIndex = 9; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMinEdit, info);
    info.columnIndex = 0; info.rowIndex = 10; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMaxText, info);
    info.columnIndex = 1; info.rowIndex = 10; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMaxSlider, info);
    info.columnIndex = 2; info.rowIndex = 10; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colorMaxEdit, info);
    info.columnIndex = 0; info.rowIndex = 12; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 1; info.rowIndex = 12; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_okButton, info);
}

RibbonProbeWindow::~RibbonProbeWindow()
{
    g_RibbonProbeWindow = NULL;

    delete m_okButton;
    delete m_cancelButton;
    delete m_colorMinEdit;
    delete m_colorMinSlider;
    delete m_colorMinText;
    delete m_colorMaxEdit;
    delete m_colorMaxSlider;
    delete m_colorMaxText;
    delete m_colorRangeCheck;
    delete m_colorRangeText;
    delete m_criteriaCombo;
    delete m_criteriaText;
    delete m_colormapCombo;
    delete m_colormapText;
    delete m_ribbonWidthEdit;
    delete m_ribbonWidthSlider;
    delete m_ribbonWidthText;
    delete m_ribbonCountEdit;
    delete m_ribbonCountSlider;
    delete m_ribbonCountText;
    delete m_probeHeightEdit;
    delete m_probeHeightSlider;
    delete m_probeHeightText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void RibbonProbeWindow::showWindow()
{
    // you can't open a RibbonProbeWindow without a probe
    return;
}

void RibbonProbeWindow::showWindow(Vector2 position)
{
    // you can't open a RibbonProbeWindow without a probe
    return;
}

void RibbonProbeWindow::showWindow(RibbonProbe* probe)
{
    this->showWindow(getScreenCenter(), probe);
}

void RibbonProbeWindow::showWindow(Vector2 position, RibbonProbe* probe)
{
    if (!(this->isHidden()))
        return;
        
    if (!probe)
        return;

    m_probe = probe;
    char text[50];
    sprintf(text, "Edit Ribbon Probe (Probe %i)", probe->id());
    this->setText(text);

    // calculate offset and scale for colorRangeSlider
    double min = probe->dataMinimum();
    double max = probe->dataMaximum();
    m_colorRangeScale = 10000.0f / (max - min);
    m_colorRangeOffset = min;

    // store original values
    m_savedProbeHeight = probe->emitterHeight();
    
    // set slider borders
    double scale = g_Application->model()->scalingFactor() * 4;
    m_probeHeightSlider->setMaximum((int)scale);

    // set Controlls
    probeHeightChanged(probe->emitterHeight(), true, true);
    ribbonCountChanged(probe->numRibbons(), true, true);
    ribbonWidthChanged(probe->ribbonHeight(), true, true);
    colorMinChanged(probe->colorRangeMin(), true, true);
    colorMaxChanged(probe->colorRangeMax(), true, true);
    
    m_colormapCombo->selectStringAt(probe->colorRangeIndex());

    switch (probe->sampleType()) {
    case SAMPLETYPE_DENSITY:
        m_criteriaCombo->selectStringAt(0);
        break;

    case SAMPLETYPE_PRESSURE:
        m_criteriaCombo->selectStringAt(1);
        break;

    case SAMPLETYPE_VELOCITY:
        m_criteriaCombo->selectStringAt(2);
        break;

    default:
        break;
    }

    if (probe->customMinMax())
        m_colorRangeCheck->enableChecked();
    else 
        m_colorRangeCheck->disableChecked();
    colorRangeCheckClicked();
    
    StandardWindow::showWindow(position);
}

void RibbonProbeWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void RibbonProbeWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_okButton->enable();
    m_cancelButton->enable();
    m_colorMinEdit->enable();
    m_colorMinSlider->enable();
    m_colorMinText->enable();
    m_colorMaxEdit->enable();
    m_colorMaxSlider->enable();
    m_colorMaxText->enable();
    m_colorRangeCheck->enable();
    m_colorRangeText->enable();
    m_criteriaCombo->enable();
    m_criteriaText->enable();
    m_colormapCombo->enable();
    m_colormapText->enable();
    m_ribbonWidthEdit->enable();
    m_ribbonWidthSlider->enable();
    m_ribbonWidthText->enable();
    m_ribbonCountEdit->enable();
    m_ribbonCountSlider->enable();
    m_ribbonCountText->enable();
    m_probeHeightEdit->enable();
    m_probeHeightSlider->enable();
    m_probeHeightText->enable();
    
    StandardWindow::enableWindow();
}

void RibbonProbeWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_okButton->disable();
    m_cancelButton->disable();
    m_colorMinEdit->disable();
    m_colorMinSlider->disable();
    m_colorMinText->disable();
    m_colorMaxEdit->disable();
    m_colorMaxSlider->disable();
    m_colorMaxText->disable();
    m_colorRangeCheck->disable();
    m_colorRangeText->disable();
    m_criteriaCombo->disable();
    m_criteriaText->disable();
    m_colormapCombo->disable();
    m_colormapText->disable();
    m_ribbonWidthEdit->disable();
    m_ribbonWidthSlider->disable();
    m_ribbonWidthText->disable();
    m_ribbonCountEdit->disable();
    m_ribbonCountSlider->disable();
    m_ribbonCountText->disable();
    m_probeHeightEdit->disable();
    m_probeHeightSlider->disable();
    m_probeHeightText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void RibbonProbeWindow::probeHeightSliderMoved(int value)
{
    probeHeightChanged(value, false, true);
}

void RibbonProbeWindow::probeHeightEditReturnPressed()
{
    double probeHeight;
    if (getValueFromEdit(m_probeHeightEdit, probeHeight, 0.0f, -1.0f, "Please insert a valid probe height (>1)"))
        probeHeightChanged(probeHeight, true, false);
}

void RibbonProbeWindow::ribbonCountSliderMoved(int value)
{
    ribbonCountChanged(value, false, true);
}

void RibbonProbeWindow::ribbonCountEditReturnPressed()
{
    int count;
    if (getValueFromEdit(m_ribbonCountEdit, count, 1, 50, "Please insert a valid ribbon count (1-50)"))
        ribbonCountChanged(count, true, false);
}

void RibbonProbeWindow::ribbonWidthSliderMoved(int value)
{
    ribbonWidthChanged(value, false, true);
}

void RibbonProbeWindow::ribbonWidthEditReturnPressed()
{
    double width;
    if (getValueFromEdit(m_ribbonWidthEdit, width, 0.0f, 100.0f, "Please insert a valid ribbon width (0-100)"))
        ribbonWidthChanged(width, true, false);
}

void RibbonProbeWindow::cancelButtonPressed()
{
    m_probe->setEmitterHeight(m_savedProbeHeight);
    this->hideWindow();
}

void RibbonProbeWindow::colorRangeCheckClicked()
{
    if (m_colorRangeCheck->isChecked())
    {
        m_colorMinText->show();
        m_colorMinSlider->show();
        m_colorMinEdit->show();
        m_colorMaxText->show();
        m_colorMaxSlider->show();
        m_colorMaxEdit->show();
    } else {
        m_colorMinText->hide();
        m_colorMinSlider->hide();
        m_colorMinEdit->hide();
        m_colorMaxText->hide();
        m_colorMaxSlider->hide();
        m_colorMaxEdit->hide();
    }
}

void RibbonProbeWindow::colorMinSliderMoved(int value)
{
    colorMinChanged(value/m_colorRangeScale + m_colorRangeOffset, false, true);
}

void RibbonProbeWindow::colorMinEditReturnPressed()
{
    double min;
    if (getValueFromEdit(m_colorMinEdit, min, -1.0f, -1.0f, "Please insert a valid minimum"))
        colorMinChanged(min, true, false);
}

void RibbonProbeWindow::colorMaxSliderMoved(int value)
{
    colorMaxChanged(value/m_colorRangeScale + m_colorRangeOffset, false, true);
}

void RibbonProbeWindow::colorMaxEditReturnPressed()
{
    double max;
    if (getValueFromEdit(m_colorMaxEdit, max, -1.0f, -1.0f, "Please insert a valid maximum"))
        colorMaxChanged(max, true, false);
}

void RibbonProbeWindow::okButtonPressed()
{
    // update Edit-Fields
    getValueFromEdit(m_ribbonCountEdit, m_temporaryRibbonCount, 1, 50, "");
    getValueFromEdit(m_ribbonWidthEdit, m_temporaryRibbonWidth, 0.0f, 100.0f, "");
    getValueFromEdit(m_probeHeightEdit, m_temporaryProbeHeight, 1.0f, -1.0f, "");
    if (m_colorRangeCheck->isChecked()) {
        getValueFromEdit(m_colorMinEdit, m_temporaryColorMin, -1.0f, -1.0f, "");
        getValueFromEdit(m_colorMaxEdit, m_temporaryColorMax, -1.0f, -1.0f, "");
    }

    // set values
    m_probe->setNumRibbons(m_temporaryRibbonCount);
    m_probe->setRibbonHeight(m_temporaryRibbonWidth);
    m_probe->setEmitterHeight(m_temporaryProbeHeight);

    m_probe->setColorRangeIndex(m_colormapCombo->selectedIndex());

    switch (m_criteriaCombo->selectedIndex()) {
    case 0:
        m_probe->setSampleType(SAMPLETYPE_DENSITY);
        break;

    case 1:
        m_probe->setSampleType(SAMPLETYPE_PRESSURE);
        break;

    case 2:
        m_probe->setSampleType(SAMPLETYPE_VELOCITY);
        break;

    default:
        break;
    }

    m_probe->setCustomMinMax(m_colorRangeCheck->isChecked());
    if (m_colorRangeCheck->isChecked()) {
        m_probe->setColorRangeMin(m_temporaryColorMin);
        m_probe->setColorRangeMax(m_temporaryColorMax);
    }

    // notify simulation
    m_probe->notifySimulation();
    // close window
    this->hideWindow();
}

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

void RibbonProbeWindow::probeHeightChanged(double probeHeight, bool setSlider, bool setEdit) {
    m_temporaryProbeHeight = probeHeight;
    if (setSlider)
        m_probeHeightSlider->setValue((int)probeHeight);
    if (setEdit) {
        char text[10];
        sprintf(text, "%.2f", probeHeight);
        m_probeHeightEdit->setText(text);
    }
    m_probe->setEmitterHeight(probeHeight);
}

void RibbonProbeWindow::ribbonCountChanged(int ribbonCount, bool setSlider, bool setEdit) {
    m_temporaryRibbonCount = ribbonCount;
    if (setSlider)
        m_ribbonCountSlider->setValue(ribbonCount);
    if (setEdit) {
        char text[10];
        sprintf(text, "%i", ribbonCount);
        m_ribbonCountEdit->setText(text);
    }
}

void RibbonProbeWindow::ribbonWidthChanged(double ribbonWidth, bool setSlider, bool setEdit) {
    m_temporaryRibbonWidth = ribbonWidth;
    if (setSlider)
        m_ribbonWidthSlider->setValue((int)ribbonWidth);
    if (setEdit) {
        char text[10];
        sprintf(text, "%.2f", ribbonWidth);
        m_ribbonWidthEdit->setText(text);
    }
}

void RibbonProbeWindow::colorMinChanged(double colorMin, bool setSlider, bool setEdit) {
    m_temporaryColorMin = colorMin;
    if (setSlider)
        m_colorMinSlider->setValue((int)((colorMin-m_colorRangeOffset)*m_colorRangeScale));
    if (setEdit) {
        char text[10];
        sprintf(text, "%.5f", colorMin);
        m_colorMinEdit->setText(text);
    }
}

void RibbonProbeWindow::colorMaxChanged(double colorMax, bool setSlider, bool setEdit) {
    m_temporaryColorMax = colorMax;
    if (setSlider)
        m_colorMaxSlider->setValue((int)((colorMax-m_colorRangeOffset)*m_colorRangeScale));
    if (setEdit) {
        char text[10];
        sprintf(text, "%.5f", colorMax);
        m_colorMaxEdit->setText(text);
    }
}
