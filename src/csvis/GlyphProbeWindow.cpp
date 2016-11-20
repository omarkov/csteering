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

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "GlyphProbeWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

GlyphProbeWindow* g_GlyphProbeWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

GlyphProbeWindow::GlyphProbeWindow()
{
    g_GlyphProbeWindow = this;

    this->setSize(Vector2(600, 290));
    this->setText("Edit Glyph Probe");

    // Create objects 
    m_probeHeightText = new StaticText(NULL, "Probe height:");
    m_probeHeightSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_probeHeightSlider->setMinimum(1);
    m_probeHeightEdit = new EditField(NULL, "");
    m_glyphCountText = new StaticText(NULL, "Glyph count:");
    m_glyphCountSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_glyphCountSlider->setMinimum(1);
    m_glyphCountSlider->setMaximum(10);
    m_glyphCountEdit = new EditField(NULL, "");
    m_colormapText = new StaticText(NULL, "Colormap:");
    m_colormapCombo = new ComboBox(NULL, Rectangle(10, 10, 20, 20));
    m_colormapCombo->addString("Temperature");
    m_colormapCombo->addString("Magenta");
    m_colormapCombo->addString("Rainbow");
    m_colormapCombo->addString("Blue-Red");
    m_colormapCombo->addString("Blue-White");
    m_colormapCombo->addString("Green-Yellow-Red");
    m_criteriaText = new StaticText(NULL, "Criteria:");
    m_criteriaCombo = new ComboBox(NULL, Rectangle(10, 10, 20, 20));
    m_criteriaCombo->addString("Pressure");
    m_criteriaCombo->addString("Velocity");
    m_cancelButton = new Button(NULL, "Cancel");
    m_okButton = new Button(NULL, "OK");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_probeHeightSlider->sliderMoved.connect(this, &GlyphProbeWindow::probeHeightSliderMoved);
    m_probeHeightEdit->returnPressed.connect(this, &GlyphProbeWindow::probeHeightEditReturnPressed);
    m_glyphCountSlider->sliderMoved.connect(this, &GlyphProbeWindow::glyphCountSliderMoved);
    m_glyphCountEdit->returnPressed.connect(this, &GlyphProbeWindow::glyphCountEditReturnPressed);
    m_cancelButton->pressed.connect(this, &GlyphProbeWindow::cancelButtonPressed);
    m_okButton->pressed.connect(this, &GlyphProbeWindow::okButtonPressed);

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
    m_panel->addChildWindow(m_glyphCountText, info);
    info.columnIndex = 1; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_glyphCountSlider, info);
    info.columnIndex = 2; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_glyphCountEdit, info);
    info.columnIndex = 0; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colormapText, info);
    info.columnIndex = 1; info.rowIndex = 3; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_colormapCombo, info);
    info.columnIndex = 0; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_criteriaText, info);
    info.columnIndex = 1; info.rowIndex = 4; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_criteriaCombo, info);
    info.columnIndex = 0; info.rowIndex = 6; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 1; info.rowIndex = 6; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_okButton, info);
}

GlyphProbeWindow::~GlyphProbeWindow()
{
    g_GlyphProbeWindow = NULL;

    delete m_okButton;
    delete m_cancelButton;
    delete m_criteriaCombo;
    delete m_criteriaText;
    delete m_colormapCombo;
    delete m_colormapText;
    delete m_glyphCountEdit;
    delete m_glyphCountSlider;
    delete m_glyphCountText;
    delete m_probeHeightEdit;
    delete m_probeHeightSlider;
    delete m_probeHeightText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void GlyphProbeWindow::showWindow()
{
    // you can't open a GlyphProbeWindow without a probe
    return;
}

void GlyphProbeWindow::showWindow(Vector2 position)
{
    // you can't open a GlyphProbeWindow without a probe
    return;
}

void GlyphProbeWindow::showWindow(GlyphProbe* probe)
{
    this->showWindow(getScreenCenter(), probe);
}

void GlyphProbeWindow::showWindow(Vector2 position, GlyphProbe* probe)
{
    if (!(this->isHidden()))
        return;
        
    if (!probe)
        return;

    m_probe = probe;
    char text[50];
    sprintf(text, "Edit Glyph Probe (Probe %i)", probe->id());
    this->setText(text);

    // store original values
    m_savedProbeHeight = probe->emitterHeight();
    
    // set slider borders
    double scale = g_Application->model()->scalingFactor();
    m_probeHeightSlider->setMaximum((int)scale);
    
    // set Controlls
    probeHeightChanged(probe->emitterHeight(), true, true);
    glyphCountChanged(probe->numStreams(), true, true);
    m_colormapCombo->selectStringAt(probe->colorRangeIndex());
    if (probe->sampleType() == SAMPLETYPE_PRESSURE)
        m_criteriaCombo->selectStringAt(0);
    else 
        m_criteriaCombo->selectStringAt(1);
    
    StandardWindow::showWindow(position);
}

void GlyphProbeWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void GlyphProbeWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_okButton->enable();
    m_cancelButton->enable();
    m_criteriaCombo->enable();
    m_criteriaText->enable();
    m_colormapCombo->enable();
    m_colormapText->enable();
    m_glyphCountEdit->enable();
    m_glyphCountSlider->enable();
    m_glyphCountText->enable();
    m_probeHeightEdit->enable();
    m_probeHeightSlider->enable();
    m_probeHeightText->enable();
    
    StandardWindow::enableWindow();
}

void GlyphProbeWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_okButton->disable();
    m_cancelButton->disable();
    m_criteriaCombo->disable();
    m_criteriaText->disable();
    m_colormapCombo->disable();
    m_colormapText->disable();
    m_glyphCountEdit->disable();
    m_glyphCountSlider->disable();
    m_glyphCountText->disable();
    m_probeHeightEdit->disable();
    m_probeHeightSlider->disable();
    m_probeHeightText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void GlyphProbeWindow::probeHeightSliderMoved(int value)
{
    probeHeightChanged(value, false, true);
}

void GlyphProbeWindow::probeHeightEditReturnPressed()
{
    double probeHeight;
    if (getValueFromEdit(m_probeHeightEdit, probeHeight, 0.0f, -1.0f, "Please insert a valid probe height (>1)"))
        probeHeightChanged(probeHeight, true, false);
}

void GlyphProbeWindow::glyphCountSliderMoved(int value)
{
    glyphCountChanged(value, false, true);
}

void GlyphProbeWindow::glyphCountEditReturnPressed()
{
    int count;
    if (getValueFromEdit(m_glyphCountEdit, count, 1, 10, "Please insert a valid glyph count (1-10)"))
        glyphCountChanged(count, true, false);
}

void GlyphProbeWindow::cancelButtonPressed()
{
    m_probe->setEmitterHeight(m_savedProbeHeight);
    this->hideWindow();
}

void GlyphProbeWindow::okButtonPressed()
{
    // update Edit-Fields
    getValueFromEdit(m_glyphCountEdit, m_temporaryGlyphCount, 1, 10, "");
    getValueFromEdit(m_probeHeightEdit, m_temporaryProbeHeight, 1.0f, -1.0f, "");
    // set values
    m_probe->setNumStreams(m_temporaryGlyphCount);
    m_probe->setEmitterHeight(m_temporaryProbeHeight);
    m_probe->setColorRangeIndex(m_colormapCombo->selectedIndex());
    if (m_criteriaCombo->selectedIndex() == 0)
        m_probe->setSampleType(SAMPLETYPE_PRESSURE);
    else 
        m_probe->setSampleType(SAMPLETYPE_VELOCITY);
    // notify simulation
    m_probe->notifySimulation();
    // close window
    this->hideWindow();
}

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

void GlyphProbeWindow::probeHeightChanged(double probeHeight, bool setSlider, bool setEdit) {
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

void GlyphProbeWindow::glyphCountChanged(int glyphCount, bool setSlider, bool setEdit) {
    m_temporaryGlyphCount = glyphCount;
    if (setSlider)
        m_glyphCountSlider->setValue(glyphCount);
    if (setEdit) {
        char text[10];
        sprintf(text, "%i", glyphCount);
        m_glyphCountEdit->setText(text);
    }
}
