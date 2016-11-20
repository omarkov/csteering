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

#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <string>

#include "glgooey/FrameWindow.h"
#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/StaticText.h"
#include "glgooey/Button.h"
#include "glgooey/ComboBox.h"
#include "glgooey/Slider.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "StandardWindow.h"

class PreferencesWindow : public StandardWindow
{
    Gooey::ComplexGridLayouter* m_layouter;
    Gooey::StaticText* m_cursorStyleText;
    Gooey::ComboBox* m_cursorStyleCombo;
    Gooey::StaticText* m_cursorSizeText;
    Gooey::Slider* m_cursorSizeSlider;
    Gooey::Button* m_cancelButton;
    Gooey::Button* m_okButton;

    unsigned int m_cursorStyle;
    unsigned int m_cursorSize;
    
public:
    // Constructor
    PreferencesWindow();
    // Destructor
    ~PreferencesWindow();

    void showWindow();
    void showWindow(Gooey::Core::Vector2 position);
    void hideWindow();
    void enableWindow();
    void disableWindow();

private:
    void cursorStyleComboChanged(const std::string& itemText);
    void cursorSizeSliderChanged(int value);
    void cancelButtonPressed();
    void okButtonPressed();
};

extern PreferencesWindow* g_PreferencesWindow;

#endif // PREFERENCESWINDOW_H
