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

#include <math.h>

#include "Application.h"
#include "GUIController.h"

#include "CommonServer.h"
#include "FANClasses.h"
#include "common.h"

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/EditField.h"
#include "glgooey/Slider.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "SimulationWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

SimulationWindow* g_SimulationWindow = NULL;

const double DENSITY = 0.5f;
const double ACCELERATION = 0.05f;
const double RELAXIATION = 1.85f;

const double SIZE_X = 3.4f;
const double SIZE_Y = 1.7f;
const double SIZE_Z = 1.7f;

const unsigned int RESOLUTION = 6;
const unsigned int UPDATE = 10;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

SimulationWindow::SimulationWindow()
{
    g_SimulationWindow = this;

    this->setSize(Vector2(700, 660));
    this->setText("Setup Simulation");

    // Create objects 
    m_header1 = new StaticText(NULL, "Simulation Parameter");
    m_densityText = new StaticText(NULL, "Density:");
    m_densitySlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_densitySlider->setMinimum(-500);
    m_densitySlider->setMaximum(500);
    m_densityEdit = new EditField(NULL, "");
    m_densityButton = new Gooey::Button(NULL, "Default");
    m_accelerationText = new StaticText(NULL, "Acceleration:");
    m_accelerationSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_accelerationSlider->setMinimum(-500);
    m_accelerationSlider->setMaximum(500);
    m_accelerationEdit = new EditField(NULL, "");
    m_accelerationButton = new Gooey::Button(NULL, "Default");
    m_relaxationText = new StaticText(NULL, "Relaxation:");
    m_relaxationSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_relaxationSlider->setMinimum(-500);
    m_relaxationSlider->setMaximum(500);
    m_relaxationEdit = new EditField(NULL, "");
    m_relaxationButton = new Gooey::Button(NULL, "Default");
    m_header2 = new StaticText(NULL, "Model Rotation");
    m_xRotationText = new StaticText(NULL, "Rotation X:");
    m_xRotationSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_xRotationSlider->setMinimum(-180);
    m_xRotationSlider->setMaximum(180);
    m_xRotationEdit = new EditField(NULL, "0");
    m_yRotationText = new StaticText(NULL, "Rotation Y:");
    m_yRotationSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_yRotationSlider->setMinimum(-180);
    m_yRotationSlider->setMaximum(180);
    m_yRotationEdit = new EditField(NULL, "0");
    m_zRotationText = new StaticText(NULL, "Rotation Z:");
    m_zRotationSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_zRotationSlider->setMinimum(-180);
    m_zRotationSlider->setMaximum(180);
    m_zRotationEdit = new EditField(NULL, "0");
    m_header3 = new StaticText(NULL, "Simulation Area");
    m_xSizeText = new StaticText(NULL, "Size X:");
    m_xSizeSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_xSizeSlider->setMinimum(100);
    m_xSizeSlider->setMaximum(1000);
    m_xSizeEdit = new EditField(NULL, "0");
    m_ySizeText = new StaticText(NULL, "Size Y:");
    m_ySizeSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_ySizeSlider->setMinimum(100);
    m_ySizeSlider->setMaximum(1000);
    m_ySizeEdit = new EditField(NULL, "0");
    m_zSizeText = new StaticText(NULL, "Size Z:");
    m_zSizeSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_zSizeSlider->setMinimum(100);
    m_zSizeSlider->setMaximum(1000);
    m_zSizeEdit = new EditField(NULL, "0");
    m_resolutionText = new StaticText(NULL, "Resolution:");
    m_resolutionSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_resolutionSlider->setMinimum(5);
    m_resolutionSlider->setMaximum(8);
    m_resolutionEdit = new EditField(NULL, "0");
    m_updateText = new StaticText(NULL, "Update Rate:");
    m_updateSlider = new Slider(NULL, Slider::horizontal, Rectangle(10, 10, 20, 20));
    m_updateSlider->setMinimum(1);
    m_updateSlider->setMaximum(25);
    m_updateEdit = new EditField(NULL, "0");
    m_cancelButton = new Gooey::Button(NULL, "Cancel");
    m_okButton = new Gooey::Button(NULL, "OK");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_densitySlider->sliderMoved.connect(this, &SimulationWindow::densitySliderMoved);
    m_densitySlider->sliderReleased.connect(this, &SimulationWindow::densitySliderReleased);
    m_densityEdit->returnPressed.connect(this, &SimulationWindow::densityEditReturnPressed);
    m_densityButton->pressed.connect(this, &SimulationWindow::densityButtonPressed);
    m_accelerationSlider->sliderMoved.connect(this, &SimulationWindow::accelerationSliderMoved);
    m_accelerationSlider->sliderReleased.connect(this, &SimulationWindow::accelerationSliderReleased);
    m_accelerationEdit->returnPressed.connect(this, &SimulationWindow::accelerationEditReturnPressed);
    m_accelerationButton->pressed.connect(this, &SimulationWindow::accelerationButtonPressed);
    m_relaxationSlider->sliderMoved.connect(this, &SimulationWindow::relaxationSliderMoved);
    m_relaxationSlider->sliderReleased.connect(this, &SimulationWindow::relaxationSliderReleased);
    m_relaxationEdit->returnPressed.connect(this, &SimulationWindow::relaxationEditReturnPressed);
    m_relaxationButton->pressed.connect(this, &SimulationWindow::relaxationButtonPressed);
    m_xRotationSlider->sliderMoved.connect(this, &SimulationWindow::xRotationSliderMoved);
    m_xRotationEdit->returnPressed.connect(this, &SimulationWindow::xRotationEditReturnPressed);
    m_yRotationSlider->sliderMoved.connect(this, &SimulationWindow::yRotationSliderMoved);
    m_yRotationEdit->returnPressed.connect(this, &SimulationWindow::yRotationEditReturnPressed);
    m_zRotationSlider->sliderMoved.connect(this, &SimulationWindow::zRotationSliderMoved);
    m_zRotationEdit->returnPressed.connect(this, &SimulationWindow::zRotationEditReturnPressed);
    m_xSizeSlider->sliderMoved.connect(this, &SimulationWindow::xSizeSliderMoved);
    m_xSizeEdit->returnPressed.connect(this, &SimulationWindow::xSizeEditReturnPressed);
    m_ySizeSlider->sliderMoved.connect(this, &SimulationWindow::ySizeSliderMoved);
    m_ySizeEdit->returnPressed.connect(this, &SimulationWindow::ySizeEditReturnPressed);
    m_zSizeSlider->sliderMoved.connect(this, &SimulationWindow::zSizeSliderMoved);
    m_zSizeEdit->returnPressed.connect(this, &SimulationWindow::zSizeEditReturnPressed);
    m_resolutionSlider->sliderMoved.connect(this, &SimulationWindow::resolutionSliderMoved);
    m_resolutionEdit->returnPressed.connect(this, &SimulationWindow::resolutionEditReturnPressed);
    m_updateSlider->sliderMoved.connect(this, &SimulationWindow::updateSliderMoved);
    m_updateEdit->returnPressed.connect(this, &SimulationWindow::updateEditReturnPressed);
    m_cancelButton->pressed.connect(this, &SimulationWindow::cancelButtonPressed);
    m_okButton->pressed.connect(this, &SimulationWindow::okButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(0.0f);
    widths.push_back(280.0f);
    widths.push_back(100.0f);
    widths.push_back(110.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(10.0f); // space
    heights.push_back(30.0f); // header
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(15.0f); // space
    heights.push_back(30.0f); // header
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(15.0f); // space
    heights.push_back(30.0f); // header
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(20.0f); // space
    heights.push_back(35.0f);
    heights.push_back(35.0f);
    heights.push_back(20.0f); // space
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 4; info.verticalSpan = 1;
    m_panel->addChildWindow(m_header1, info);
    info.columnIndex = 0; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_densityText, info);
    info.columnIndex = 1; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_densitySlider, info);
    info.columnIndex = 2; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_densityEdit, info);
    info.columnIndex = 3; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_densityButton, info);
    info.columnIndex = 0; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_accelerationText, info);
    info.columnIndex = 1; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_accelerationSlider, info);
    info.columnIndex = 2; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_accelerationEdit, info);
    info.columnIndex = 3; info.rowIndex = 3; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_accelerationButton, info);
    info.columnIndex = 0; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_relaxationText, info);
    info.columnIndex = 1; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_relaxationSlider, info);
    info.columnIndex = 2; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_relaxationEdit, info);
    info.columnIndex = 3; info.rowIndex = 4; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_relaxationButton, info);
    info.columnIndex = 0; info.rowIndex = 6; info.horizontalSpan = 4; info.verticalSpan = 1;
    m_panel->addChildWindow(m_header2, info);
    info.columnIndex = 0; info.rowIndex = 7; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xRotationText, info);
    info.columnIndex = 1; info.rowIndex = 7; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xRotationSlider, info);
    info.columnIndex = 3; info.rowIndex = 7; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xRotationEdit, info);
    info.columnIndex = 0; info.rowIndex = 8; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_yRotationText, info);
    info.columnIndex = 1; info.rowIndex = 8; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_yRotationSlider, info);
    info.columnIndex = 3; info.rowIndex = 8; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_yRotationEdit, info);
    info.columnIndex = 0; info.rowIndex = 9; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zRotationText, info);
    info.columnIndex = 1; info.rowIndex = 9; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zRotationSlider, info);
    info.columnIndex = 3; info.rowIndex = 9; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zRotationEdit, info);
    info.columnIndex = 0; info.rowIndex = 11; info.horizontalSpan = 4; info.verticalSpan = 1;
    m_panel->addChildWindow(m_header3, info);
    info.columnIndex = 0; info.rowIndex = 12; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xSizeText, info);
    info.columnIndex = 1; info.rowIndex = 12; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xSizeSlider, info);
    info.columnIndex = 3; info.rowIndex = 12; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_xSizeEdit, info);
    info.columnIndex = 0; info.rowIndex = 13; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ySizeText, info);
    info.columnIndex = 1; info.rowIndex = 13; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ySizeSlider, info);
    info.columnIndex = 3; info.rowIndex = 13; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_ySizeEdit, info);
    info.columnIndex = 0; info.rowIndex = 14; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zSizeText, info);
    info.columnIndex = 1; info.rowIndex = 14; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zSizeSlider, info);
    info.columnIndex = 3; info.rowIndex = 14; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_zSizeEdit, info);
    info.columnIndex = 0; info.rowIndex = 16; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_resolutionText, info);
    info.columnIndex = 1; info.rowIndex = 16; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_resolutionSlider, info);
    info.columnIndex = 3; info.rowIndex = 16; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_resolutionEdit, info);
    info.columnIndex = 0; info.rowIndex = 17; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_updateText, info);
    info.columnIndex = 1; info.rowIndex = 17; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_updateSlider, info);
    info.columnIndex = 3; info.rowIndex = 17; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_updateEdit, info);
    info.columnIndex = 0; info.rowIndex = 19; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 1; info.rowIndex = 19; info.horizontalSpan = 3; info.verticalSpan = 1;
    m_panel->addChildWindow(m_okButton, info);
    
    m_density = DENSITY;
    m_acceleration = ACCELERATION;
    m_relaxation = RELAXIATION;
    m_xSize = SIZE_X;
    m_ySize = SIZE_Y;
    m_zSize = SIZE_Z;
    m_resolution = RESOLUTION;
    m_update = UPDATE;
}

SimulationWindow::~SimulationWindow()
{
    g_SimulationWindow = NULL;

    delete m_okButton;
    delete m_cancelButton;
    delete m_updateEdit;
    delete m_updateSlider;
    delete m_updateText;
    delete m_resolutionEdit;
    delete m_resolutionSlider;
    delete m_resolutionText;
    delete m_xSizeEdit;
    delete m_xSizeSlider;
    delete m_xSizeText;
    delete m_ySizeEdit;
    delete m_ySizeSlider;
    delete m_ySizeText;
    delete m_zSizeEdit;
    delete m_zSizeSlider;
    delete m_zSizeText;
    delete m_xRotationEdit;
    delete m_xRotationSlider;
    delete m_xRotationText;
    delete m_yRotationEdit;
    delete m_yRotationSlider;
    delete m_yRotationText;
    delete m_zRotationEdit;
    delete m_zRotationSlider;
    delete m_zRotationText;
    delete m_relaxationButton;
    delete m_relaxationEdit;
    delete m_relaxationSlider;
    delete m_relaxationText;
    delete m_accelerationButton;
    delete m_accelerationEdit;
    delete m_accelerationSlider;
    delete m_accelerationText;
    delete m_densityButton;
    delete m_densityEdit;
    delete m_densitySlider;
    delete m_densityText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void SimulationWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void SimulationWindow::showWindow(Vector2 position)
{
    if (!(this->isHidden()))
        return;

    // window-specific stuff
    char text[10];
    // parameter
    setDensity(m_density);
    setAcceleration(m_acceleration);
    setRelaxation(m_relaxation);
    // rotation
    m_originalRotation = g_Application->model()->attitude();
    double rx;
    double ry;
    double rz;
    quat2euler(m_originalRotation, rx, ry, rz);
	m_xRotationSlider->setValue((int)rx);
	sprintf(text, "%.2f", rx);
	m_xRotationEdit->setText(text);
	m_yRotationSlider->setValue((int)ry);
	sprintf(text, "%.2f", ry);
	m_yRotationEdit->setText(text);
	m_zRotationSlider->setValue((int)rz);
	sprintf(text, "%.2f", rz);
	m_zRotationEdit->setText(text);
    // size
    xSizeChanged(m_xSize, true, true);
    ySizeChanged(m_ySize, true, true);
    zSizeChanged(m_zSize, true, true);
    // resolution
    resolutionChanged(m_resolution, true, true);
    // update rate
    updateChanged(m_update, true, true);
    
    StandardWindow::showWindow(position);
}

void SimulationWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void SimulationWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_okButton->enable();
    m_cancelButton->enable();
    m_updateText->enable();
    m_updateEdit->enable();
    m_updateSlider->enable();
    m_resolutionText->enable();
    m_resolutionEdit->enable();
    m_resolutionSlider->enable();
    m_densityText->enable();
    m_densityButton->enable();
    m_densityEdit->enable();
    m_densitySlider->enable();
    m_densityText->enable();
    m_accelerationButton->enable();
    m_accelerationEdit->enable();
    m_accelerationSlider->enable();
    m_accelerationText->enable();
    m_relaxationButton->enable();
    m_relaxationEdit->enable();
    m_relaxationSlider->enable();
    m_relaxationText->enable();
    m_xRotationEdit->enable();
    m_xRotationSlider->enable();
    m_xRotationText->enable();
    m_yRotationEdit->enable();
    m_yRotationSlider->enable();
    m_yRotationText->enable();
    m_zRotationEdit->enable();
    m_zRotationSlider->enable();
    m_zRotationText->enable();
    m_xSizeEdit->enable();
    m_xSizeSlider->enable();
    m_xSizeText->enable();
    m_ySizeEdit->enable();
    m_ySizeSlider->enable();
    m_ySizeText->enable();
    m_zSizeEdit->enable();
    m_zSizeSlider->enable();
    m_zSizeText->enable();
    
    StandardWindow::enableWindow();
}

void SimulationWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_okButton->disable();
    m_cancelButton->disable();
    m_resolutionText->disable();
    m_resolutionEdit->disable();
    m_resolutionSlider->disable();
    m_updateText->disable();
    m_updateEdit->disable();
    m_updateSlider->disable();
    m_densityButton->disable();
    m_densityEdit->disable();
    m_densitySlider->disable();
    m_densityText->disable();
    m_accelerationButton->disable();
    m_accelerationEdit->disable();
    m_accelerationSlider->disable();
    m_accelerationText->disable();
    m_relaxationButton->disable();
    m_relaxationEdit->disable();
    m_relaxationSlider->disable();
    m_relaxationText->disable();
    m_xRotationEdit->disable();
    m_xRotationSlider->disable();
    m_xRotationText->disable();
    m_yRotationEdit->disable();
    m_yRotationSlider->disable();
    m_yRotationText->disable();
    m_zRotationEdit->disable();
    m_zRotationSlider->disable();
    m_zRotationText->disable();
    m_xSizeEdit->disable();
    m_xSizeSlider->disable();
    m_xSizeText->disable();
    m_ySizeEdit->disable();
    m_ySizeSlider->disable();
    m_ySizeText->disable();
    m_zSizeEdit->disable();
    m_zSizeSlider->disable();
    m_zSizeText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void SimulationWindow::densitySliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.5f", m_temporaryDensity*(1.0f+value/1000.0f));
    m_densityEdit->setText(text);
}

void SimulationWindow::densitySliderReleased()
{
    densityEditReturnPressed();
}

void SimulationWindow::densityEditReturnPressed()
{
    double value;
    if (!getValueFromEdit(m_densityEdit, value, 0, -1, "Please insert a valid density."))
        return;
    setDensity(value);
}

void SimulationWindow::densityButtonPressed()
{
    setDensity(DENSITY);
}

void SimulationWindow::accelerationSliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.5f", m_temporaryAcceleration*(1.0f+value/1000.0f));
    m_accelerationEdit->setText(text);
}

void SimulationWindow::accelerationSliderReleased()
{
    accelerationEditReturnPressed();
}

void SimulationWindow::accelerationEditReturnPressed()
{
    double value;
    if (!getValueFromEdit(m_accelerationEdit, value, 0, -1, "Please insert a valid acceleration."))
        return;
    setAcceleration(value);
}

void SimulationWindow::accelerationButtonPressed()
{
    setAcceleration(ACCELERATION);
}

void SimulationWindow::relaxationSliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.5f", m_temporaryRelaxation*(1.0f+value/1000.0f));
    m_relaxationEdit->setText(text);
}

void SimulationWindow::relaxationSliderReleased()
{
    relaxationEditReturnPressed();
}

void SimulationWindow::relaxationEditReturnPressed()
{
    double value;
    if (!getValueFromEdit(m_relaxationEdit, value, 0, -1, "Please insert a valid relaxation."))
        return;
    setRelaxation(value);
}

void SimulationWindow::relaxationButtonPressed()
{
    setRelaxation(RELAXIATION);
}

void SimulationWindow::xRotationSliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.2f", (float)value);
    m_xRotationEdit->setText(text);
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::xRotationEditReturnPressed()
{
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::yRotationSliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.2f", (float)value);
    m_yRotationEdit->setText(text);
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::yRotationEditReturnPressed()
{
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::zRotationSliderMoved(int value)
{
    char text[10];
    sprintf(text, "%.2f", (float)value);
    m_zRotationEdit->setText(text);
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::zRotationEditReturnPressed()
{
    updateSliderAndModelRotationFromEdit(true);
}

void SimulationWindow::xSizeSliderMoved(int value)
{
    xSizeChanged(value / 100.0f, false, true);
}

void SimulationWindow::xSizeEditReturnPressed()
{
    double value;
    if (getValueFromEdit(m_xSizeEdit, value, 1.0f, 10.0f, "Please enter a valid size (1-10)."))
        {
        xSizeChanged(value, true, false);
        }
}

void SimulationWindow::ySizeSliderMoved(int value)
{
    ySizeChanged(value / 100.0f, false, true);
}

void SimulationWindow::ySizeEditReturnPressed()
{
    double value;
    if (getValueFromEdit(m_ySizeEdit, value, 1.0f, 10.0f, "Please enter a valid size (1-10)."))
        {
        ySizeChanged(value, true, false);
        }
}

void SimulationWindow::zSizeSliderMoved(int value)
{
    zSizeChanged(value / 100.0f, false, true);
}

void SimulationWindow::zSizeEditReturnPressed()
{
    double value;
    if (getValueFromEdit(m_zSizeEdit, value, 1.0f, 10.0f, "Please enter a valid size (1-10)."))
        {
        zSizeChanged(value, true, false);
        }
}

void SimulationWindow::resolutionSliderMoved(int value)
{
    resolutionChanged(value, false, true);
}

void SimulationWindow::resolutionEditReturnPressed()
{
    int value;
    if (getValueFromEdit(m_resolutionEdit, value, 5, 8, "Please enter a valid resolution (5-8)."))
        {
        resolutionChanged(value, true, false);
        }
}

void SimulationWindow::updateSliderMoved(int value)
{
    updateChanged(value, false, true);
}

void SimulationWindow::updateEditReturnPressed()
{
    int value;
    if (getValueFromEdit(m_updateEdit, value, 1, -1, "Please enter a valid update rate (>0)."))
        {
        updateChanged(value, true, false);
        }
}

void SimulationWindow::cancelButtonPressed()
{
    g_Application->model()->setAttitude(m_originalRotation);
    this->hideWindow();
}

void SimulationWindow::okButtonPressed()
{
    // get current values in the edit-fields (maybe the user typed in a new number and didnt press return)
    // dont show any error-message (just do nothing in that case)
    double newValue;
    if (getValueFromEdit(m_densityEdit, newValue, 0, -1, ""))
        setDensity(newValue);
    if (getValueFromEdit(m_accelerationEdit, newValue, 0, -1, ""))
        setAcceleration(newValue);
    if (getValueFromEdit(m_relaxationEdit, newValue, 0, -1, ""))
        setRelaxation(newValue);

    updateSliderAndModelRotationFromEdit(false);
    
    getValueFromEdit(m_xSizeEdit, m_temporaryXSize, 1.0f, 10.0f, "");
    getValueFromEdit(m_ySizeEdit, m_temporaryYSize, 1.0f, 10.0f, "");
    getValueFromEdit(m_zSizeEdit, m_temporaryZSize, 1.0f, 10.0f, "");
    
    getValueFromEdit(m_resolutionEdit, m_temporaryResolution, 5, 8, "");
    
    // set permanent values
    m_density = m_temporaryDensity;
    m_acceleration = m_temporaryAcceleration;
    m_relaxation = m_temporaryRelaxation;
    m_xSize = m_temporaryXSize;
    m_ySize = m_temporaryYSize;
    m_zSize = m_temporaryZSize;
    m_resolution = m_temporaryResolution;
    m_update = m_temporaryUpdate;
    
    // send simulation parameter
    char *value;
    char *value_x;
    char *value_y;
    char *value_z;
    if(modelConn != NULL)
    {
        asprintf(&value, "%f", m_density);	
        modelConn->rpc("model::setDensity", 1, value);
	    free(value);

        asprintf(&value, "%d", m_resolution);	
        modelConn->rpc("model::setVoxelRes", 1, value);
	    free(value);

        asprintf(&value, "%d", m_update);	
        modelConn->rpc("sim::setUpdateRate", 1, value);
	    free(value);

        asprintf(&value_x, "%f", m_xSize);	
        asprintf(&value_y, "%f", m_ySize);	
        asprintf(&value_z, "%f", m_zSize);	
        modelConn->rpc("sim::setSimScale", 3, value_x, value_y, value_z);
	    free(value_x);
	    free(value_y);
	    free(value_z);

        asprintf(&value, "%f", m_acceleration);	
        modelConn->rpc("model::setAcceleration", 1, value);
	    free(value);

        asprintf(&value, "%f", m_relaxation);	
        modelConn->rpc("model::setRelaxation", 1, value);
	    free(value);
    }

    // send model rotation
    osg::Matrixd matrix;
    g_Application->model()->attitude().get(matrix);

    double *rot = new double[9];

    for(int i=0; i<3; i++)
      for(int j=0; j<3; j++)
        rot[i*3 + j] = matrix(i, j);

    if(modelConn != NULL)
    {
      modelConn->startBinaryPush("model::setRotation");
      modelConn->binaryPush("{double}[9]", rot);
      modelConn->stopBinaryPush();
    }
    delete[] rot;

    if(modelConn != NULL)
    {
      modelConn->rpc("sim::stop");
    }
    
    this->hideWindow();
}

// ---------------------------------------------------------------------------
// helper functions
// ---------------------------------------------------------------------------

void SimulationWindow::setDensity(double d)
{
    m_temporaryDensity = d;
    m_densitySlider->setValue(0);
    char text[10];
    sprintf(text, "%.5f", d);
    m_densityEdit->setText(text);    
}

void SimulationWindow::setAcceleration(double a)
{
    m_temporaryAcceleration = a;
    m_accelerationSlider->setValue(0);
    char text[10];
    sprintf(text, "%.5f", a);
    m_accelerationEdit->setText(text);    
}

void SimulationWindow::setRelaxation(double r)
{
    m_temporaryRelaxation = r;
    m_relaxationSlider->setValue(0);
    char text[10];
    sprintf(text, "%.5f", r);
    m_relaxationEdit->setText(text);    
}

bool SimulationWindow::updateSliderAndModelRotationFromEdit(bool showError)
{
    double x;
    double y;
    double z;
    std::string errorMessage = "";
    if (showError)
        errorMessage = "Please insert a valid angle (0-360).";
    if (!getValueFromEdit(m_xRotationEdit, x, -180.0f, 180.0f, errorMessage))
        return false;
    m_xRotationSlider->setValue((int)x);
    if (!getValueFromEdit(m_yRotationEdit, y, -180.0f, 180.0f, errorMessage))
        return false;
    m_yRotationSlider->setValue((int)y);
    if (!getValueFromEdit(m_zRotationEdit, z, -180.0f, 180.0f, errorMessage))
        return false;
    m_zRotationSlider->setValue((int)z);
    // rotate model
    osg::Quat attitude;
    euler2quat(x, y, z, attitude);
    /*attitude.makeRotate(x*PI/180.0f, osg::Vec3(1.0f,0.0f,0.0f),
                        y*PI/180.0f, osg::Vec3(0.0f,1.0f,0.0f),
                        z*PI/180.0f, osg::Vec3(0.0f,0.0f,1.0f));*/
    g_Application->model()->setAttitude(attitude);
    return true;
}

void SimulationWindow::xSizeChanged(double size, bool setSlider, bool setEdit)
{
    m_temporaryXSize = size;
    if (setSlider)
        m_xSizeSlider->setValue((int)(size * 100));
    if (setEdit) {
        char text[10];
        sprintf(text, "%.2f", size);
        m_xSizeEdit->setText(text);
    }
}

void SimulationWindow::ySizeChanged(double size, bool setSlider, bool setEdit)
{
    m_temporaryYSize = size;
    if (setSlider)
        m_ySizeSlider->setValue((int)(size * 100));
    if (setEdit) {
        char text[10];
        sprintf(text, "%.2f", size);
        m_ySizeEdit->setText(text);
    }
}

void SimulationWindow::zSizeChanged(double size, bool setSlider, bool setEdit)
{
    m_temporaryZSize = size;
    if (setSlider)
        m_zSizeSlider->setValue((int)(size * 100));
    if (setEdit) {
        char text[10];
        sprintf(text, "%.2f", size);
        m_zSizeEdit->setText(text);
    }
}

void SimulationWindow::resolutionChanged(int resolution, bool setSlider, bool setEdit)
{
    m_temporaryResolution = resolution;
    if (setSlider)
        m_resolutionSlider->setValue(resolution);
    if (setEdit) {
        char text[10];
        sprintf(text, "%i", resolution);
        m_resolutionEdit->setText(text);
    }
}

void SimulationWindow::updateChanged(int update, bool setSlider, bool setEdit)
{
    m_temporaryUpdate = update;
    if (setSlider)
        m_updateSlider->setValue(update);
    if (setEdit) {
        char text[10];
        sprintf(text, "%i", update);
        m_updateEdit->setText(text);
    }
}
