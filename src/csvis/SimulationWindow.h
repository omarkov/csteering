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

#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <string>

#include "Probe.h"

#include "glgooey/FrameWindow.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/EditField.h"
#include "glgooey/Slider.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "StandardWindow.h"

class SimulationWindow : public StandardWindow
{
    double m_density, m_acceleration, m_relaxation;
    double m_temporaryDensity, m_temporaryAcceleration, m_temporaryRelaxation;
    
    osg::Quat m_originalRotation;
    
    double m_xSize, m_ySize, m_zSize;
    double m_temporaryXSize, m_temporaryYSize, m_temporaryZSize;
    
    int m_resolution, m_temporaryResolution;
    int m_update, m_temporaryUpdate;

    Gooey::ComplexGridLayouter* m_layouter;

    Gooey::StaticText* m_header1;
    Gooey::StaticText* m_densityText;
    Gooey::Slider* m_densitySlider;
    Gooey::EditField* m_densityEdit;
    Gooey::Button* m_densityButton;
    Gooey::StaticText* m_accelerationText;
    Gooey::Slider* m_accelerationSlider;
    Gooey::EditField* m_accelerationEdit;
    Gooey::Button* m_accelerationButton;
    Gooey::StaticText* m_relaxationText;
    Gooey::Slider* m_relaxationSlider;
    Gooey::EditField* m_relaxationEdit;
    Gooey::Button* m_relaxationButton;

    Gooey::StaticText* m_header2;
    Gooey::StaticText* m_xRotationText;
    Gooey::Slider* m_xRotationSlider;
    Gooey::EditField* m_xRotationEdit;
    Gooey::StaticText* m_yRotationText;
    Gooey::Slider* m_yRotationSlider;
    Gooey::EditField* m_yRotationEdit;
    Gooey::StaticText* m_zRotationText;
    Gooey::Slider* m_zRotationSlider;
    Gooey::EditField* m_zRotationEdit;

    Gooey::StaticText* m_header3;
    Gooey::StaticText* m_xSizeText;
    Gooey::Slider* m_xSizeSlider;
    Gooey::EditField* m_xSizeEdit;
    Gooey::StaticText* m_ySizeText;
    Gooey::Slider* m_ySizeSlider;
    Gooey::EditField* m_ySizeEdit;
    Gooey::StaticText* m_zSizeText;
    Gooey::Slider* m_zSizeSlider;
    Gooey::EditField* m_zSizeEdit;

    Gooey::StaticText* m_resolutionText;
    Gooey::Slider* m_resolutionSlider;
    Gooey::EditField* m_resolutionEdit;
    Gooey::StaticText* m_updateText;
    Gooey::Slider* m_updateSlider;
    Gooey::EditField* m_updateEdit;

    Gooey::Button* m_cancelButton;
    Gooey::Button* m_okButton;
    
public:
    // Constructor
    SimulationWindow();
    // Destructor
    ~SimulationWindow();

    void showWindow();
    void showWindow(Gooey::Core::Vector2 position);
    void hideWindow();
    void enableWindow();
    void disableWindow();

private:
    void densitySliderMoved(int value);
    void densitySliderReleased();
    void densityEditReturnPressed();
    void densityButtonPressed();
    void accelerationSliderMoved(int value);
    void accelerationSliderReleased();
    void accelerationEditReturnPressed();
    void accelerationButtonPressed();
    void relaxationSliderMoved(int value);
    void relaxationSliderReleased();
    void relaxationEditReturnPressed();
    void relaxationButtonPressed();
    void xRotationSliderMoved(int value);
    void xRotationEditReturnPressed();
    void yRotationSliderMoved(int value);
    void yRotationEditReturnPressed();
    void zRotationSliderMoved(int value);
    void zRotationEditReturnPressed();
    void xSizeSliderMoved(int value);
    void xSizeEditReturnPressed();
    void ySizeSliderMoved(int value);
    void ySizeEditReturnPressed();
    void zSizeSliderMoved(int value);
    void zSizeEditReturnPressed();
    void resolutionSliderMoved(int value);
    void resolutionEditReturnPressed();
    void updateSliderMoved(int value);
    void updateEditReturnPressed();
    void cancelButtonPressed();
    void okButtonPressed();

    void setDensity(double d);
    void setAcceleration(double a);
    void setRelaxation(double r);
    bool updateSliderAndModelRotationFromEdit(bool showError);
    void xSizeChanged(double size, bool setSlider, bool setEdit);
    void ySizeChanged(double size, bool setSlider, bool setEdit);
    void zSizeChanged(double size, bool setSlider, bool setEdit);
    void resolutionChanged(int resolution, bool setSlider, bool setEdit);
    void updateChanged(int update, bool setSlider, bool setEdit);
};

extern SimulationWindow* g_SimulationWindow;

#endif // SIMULATIONWINDOW_H
