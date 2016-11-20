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

#ifndef STANDARDWINDOW_H
#define STANDARDWINDOW_H

#include <string>

#include "glgooey/FrameWindow.h"
#include "glgooey/Panel.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"
#include "glgooey/EditField.h"

class StandardWindow : public Gooey::FrameWindow
{
    Gooey::WindowManager* m_WindowManager;

    bool m_isHidden;
    bool m_isDisabled;
    
protected:
    Gooey::Panel* m_panel;
    
public:
    // Constructor
    StandardWindow();
    // Destructor
    ~StandardWindow();

    Gooey::Core::Vector2 getScreenCenter();

    bool isHidden() { return m_isHidden; }
    bool isDisabled() { return m_isDisabled; }
    
    void virtual showWindow();
    void virtual showWindow(Gooey::Core::Vector2 position);
    void virtual hideWindow();
    void virtual enableWindow();
    void virtual disableWindow();
    
    bool getValueFromEdit(Gooey::EditField* edit, double& value, double min, double max, const std::string& errorMessage);
    bool getValueFromEdit(Gooey::EditField* edit, int& value, int min, int max, const std::string& errorMessage);
};

#endif // STANDARDWINDOW_H
