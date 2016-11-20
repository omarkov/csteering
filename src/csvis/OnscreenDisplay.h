// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org> 
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

#ifndef ONSCREEN_DISPLAY_H
#define ONSCREEN_DISPLAY_H

#include <osg/Geode>
#include <OpenThreads/Mutex>

#include "glgooey/services/glfont/TypeFace.h"

// namespace declarations
using Gooey::Font::TypeFace;


enum DraggingMode {
    DRAGGINGMODE_NONE = 0,
    DRAGGINGMODE_TRANSLATE,
    DRAGGINGMODE_ROTATE,
    DRAGGINGMODE_SCALE
};

class OnscreenDisplay
{
    OpenThreads::Mutex m_lock;

    osg::Vec4 m_textColor;
    osg::Vec4 m_borderColor;
    osg::Vec4 m_bgColor;

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    unsigned int m_currentFPS;
    float m_currentSPS;

    // fonts
    TypeFace* m_smallFont;
    TypeFace* m_normalFont;
    TypeFace* m_bigFont;

    // frame counter
    double m_averageFramesElapsed;
    unsigned int m_frameCount;

    // steps counter
    double m_averageStepsElapsed;
    unsigned int m_lastStepCount;
    unsigned int m_stepCount;

    // object info
    bool m_showObjectInfo;
    std::string m_objectInfoTitle;
    std::string m_objectInfo1;
    std::string m_objectInfo2;
    std::string m_objectInfo3;
    
    // server status
    bool m_isSimulationRunning;
    bool m_showServerStatus;
    std::string m_serverStatus;
    int m_serverProgress;

    // targeting reticule
    bool m_showSelection;
    osg::ref_ptr<osg::Geode> m_currentSelection;
    std::string m_selectionName;
    DraggingMode m_currentDraggingMode;

    char m_msgBuffer[1024];

private:
    /// Load the OSD fonts
    void loadFonts();
    /// Print a message
    void print(TypeFace* font, float x, float y, const char* fmt, ...);

    /// Draw the FPS/SPS counters
    void drawCounters(double elapsed);
    /// Draw the object info corner
    void drawObjectInfo(double elapsed);
    /// Draw the input corner
    void drawInputInfo(double elapsed);
    /// Draw the current GUI state
    void drawGUIState(double elapsed);
    /// Draw the server status
    void drawServerStatus(double elapsed);
    /// Draw the current selection
    void drawSelection(double elapsed);

public:
    /// Constructor
    OnscreenDisplay();
    /// Destructor
    ~OnscreenDisplay();

    /// Draw the display
    void draw(double elapsed);

    /// Set the screen size
    void setScreenSize(unsigned int width, unsigned int height);
    /// Set the current FPS
    void setCurrentFPS(unsigned int fps) { m_currentFPS = fps; }
    /// Set the current SPS
    void setCurrentSPS(float sps) { m_currentSPS = sps; }
    /// Set the number of simulation steps
    void setSimulationSteps(int steps) { m_stepCount = steps; }

    void setSimulationRunning(bool bRunning) { m_isSimulationRunning = bRunning; }
    bool isSimulationRunning() { return m_isSimulationRunning; }

    /// Call when the GUI state changed
    void changedGUIState();

    /// Show three lines of object info
    void showObjectInfo(const std::string& title,
                        const std::string& line1 = "",
                        const std::string& line2 = "",
                        const std::string& line3 = "");
    /// Hide the object info
    void hideObjectInfo() { m_showObjectInfo = false; }

    /// Hide the object info
    void showServerStatus(const std::string& status, int progress);

    void showSelectionReticule(osg::ref_ptr<osg::Geode> node, const std::string& name);
    void hideSelectionReticule();
    void setDraggingMode(DraggingMode mode) { m_currentDraggingMode = mode; }

    /// Draw the text for input info
    void drawInputDeviceText(const std::string& text, int x, int y);
    
    /// Handles mouse-clicks on specific screen-areas
    bool handleClick(unsigned int mousePosX, unsigned int mousePosY);
};

#endif // ONSCREEN_DISPLAY_H
