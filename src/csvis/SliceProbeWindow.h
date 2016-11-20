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

#ifndef SLICEPROBEWINDOW_H
#define SLICEPROBEWINDOW_H

#include <string>

#include "SliceProbe.h"

#include "glgooey/FrameWindow.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/EditField.h"
#include "glgooey/ComboBox.h"
#include "glgooey/Slider.h"
#include "glgooey/CheckBox.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "StandardWindow.h"

class SliceProbeWindow : public StandardWindow
{
    Gooey::ComplexGridLayouter* m_layouter;
    Gooey::StaticText* m_probeHeightText;
    Gooey::Slider* m_probeHeightSlider;
    Gooey::EditField* m_probeHeightEdit;
    Gooey::StaticText* m_probeWidthText;
    Gooey::Slider* m_probeWidthSlider;
    Gooey::EditField* m_probeWidthEdit;
    Gooey::StaticText* m_criteriaText;
    Gooey::ComboBox* m_criteriaCombo;
    Gooey::StaticText* m_colormapText;
    Gooey::ComboBox* m_colormapCombo;
    Gooey::StaticText* m_colorRangeText;
    Gooey::CheckBox* m_colorRangeCheck;
    Gooey::StaticText* m_colorMinText;
    Gooey::Slider* m_colorMinSlider;
    Gooey::EditField* m_colorMinEdit;
    Gooey::StaticText* m_colorMaxText;
    Gooey::Slider* m_colorMaxSlider;
    Gooey::EditField* m_colorMaxEdit;
    Gooey::Button* m_cancelButton;
    Gooey::Button* m_okButton;
    
    SliceProbe* m_probe;
    
    double m_savedProbeHeight;
    double m_temporaryProbeHeight;
    double m_savedProbeWidth;
    double m_temporaryProbeWidth;
    double m_temporaryColorMin;
    double m_temporaryColorMax;
    
    double m_colorRangeOffset;
    double m_colorRangeScale;
    
public:
    // Constructor
    SliceProbeWindow();
    // Destructor
    ~SliceProbeWindow();

    void showWindow();
    void showWindow(Gooey::Core::Vector2 position);
    void showWindow(SliceProbe* probe);
    void showWindow(Gooey::Core::Vector2 position, SliceProbe* probe);
    void hideWindow();
    void enableWindow();
    void disableWindow();

private:
    void probeHeightSliderMoved(int value);
    void probeHeightEditReturnPressed();
    void probeWidthSliderMoved(int value);
    void probeWidthEditReturnPressed();
    void colorRangeCheckClicked();
    void colorMinSliderMoved(int value);
    void colorMinEditReturnPressed();
    void colorMaxSliderMoved(int value);
    void colorMaxEditReturnPressed();
    void cancelButtonPressed();
    void okButtonPressed();
    
    void probeHeightChanged(double probeHeight, bool setSlider, bool setEdit);
    void probeWidthChanged(double probeWidth, bool setSlider, bool setEdit);
    void colorMinChanged(double colorMin, bool setSlider, bool setEdit);
    void colorMaxChanged(double colorMax, bool setSlider, bool setEdit);
};

extern SliceProbeWindow* g_SliceProbeWindow;

#endif // SLICEPROBEWINDOW_H
