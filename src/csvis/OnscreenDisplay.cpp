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
// LIABLE FOR ANY DIRECT, INDIRECT, titINCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <math.h>
#include <osg/GL>

#include "CommonServer.h"
#include "FANClasses.h"

#include "Application.h"
#include "GUIController.h"
#include "InputManager.h"
#include "OnscreenDisplay.h"
#include "common.h"


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

OnscreenDisplay::OnscreenDisplay()
    : m_screenWidth(0), m_screenHeight(0), m_currentFPS(0), m_currentSPS(0),
      m_smallFont(0), m_normalFont(0), m_bigFont(0), m_averageFramesElapsed(0.0), m_frameCount(0),
      m_averageStepsElapsed(0.0), m_lastStepCount(0), m_stepCount(0), m_showObjectInfo(false),
      m_isSimulationRunning(false), m_showServerStatus(false), m_showSelection(false),
      m_currentDraggingMode(DRAGGINGMODE_NONE)
{
    m_textColor.set(1.0f, 1.0f, 1.0f, 1.0f);
    m_borderColor.set(1.0f, 1.0f, 1.0f, 1.0f);
    m_bgColor.set(1.0f, 1.0f, 1.0f, 0.25f);

    loadFonts();
}

OnscreenDisplay::~OnscreenDisplay()
{
    // FIXME: caching bug
    /*
    zap(m_smallFont);
    zap(m_normalFont);
    zap(m_bigFont);
    */
}


// ---------------------------------------------------------------------------
// Text handling
// ---------------------------------------------------------------------------

void OnscreenDisplay::loadFonts()
{
    std::string fontPath = g_Application->dataPath("fonts/andalemo.ttf");

    m_smallFont = new TypeFace(fontPath, 10);
    m_normalFont = new TypeFace(fontPath, 12);
    m_bigFont = new TypeFace(fontPath, 14);
}

void OnscreenDisplay::print(TypeFace* font, float x, float y, const char* fmt, ...)
{
    assert(font);

    va_list args;

    va_start(args, fmt);
    vsnprintf(m_msgBuffer, 1024, fmt, args);
    va_end(args);

    font->render(m_msgBuffer, Gooey::Core::Vector2(x, y));
}


void OnscreenDisplay::showObjectInfo(const std::string& title,
                                     const std::string& line1,
                                     const std::string& line2,
                                     const std::string& line3)
{
    m_showObjectInfo = true;

    m_objectInfoTitle = title;
    m_objectInfo1 = line1;
    m_objectInfo2 = line2;
    m_objectInfo3 = line3;
}

void OnscreenDisplay::showServerStatus(const std::string& status, int progress)
{
    m_lock.lock();

    m_showServerStatus = !status.empty();
    m_serverStatus = status;
    m_serverProgress = progress;

    m_lock.unlock();
}

void OnscreenDisplay::showSelectionReticule(osg::ref_ptr<osg::Geode> node, const std::string& name)
{
    m_currentSelection = node;
    m_selectionName = name;
    m_showSelection = true;
}

void OnscreenDisplay::hideSelectionReticule()
{
    m_currentSelection = NULL;
    m_showSelection = false;
}

void OnscreenDisplay::drawInputDeviceText(const std::string& text, int x, int y)
{
    // (x,y) is the point on the right end of the text, vertically in the middle
    float tx = m_smallFont->width(text);
    float ty = m_smallFont->descent();

    print(m_smallFont, x-tx, y+ty, text.c_str());
}

// ----------------------------------------------------------------------------
// Parameters
// ----------------------------------------------------------------------------

void OnscreenDisplay::setScreenSize(unsigned int width, unsigned int height)
{
    m_screenWidth = width;
    m_screenHeight = height;
}

void OnscreenDisplay::changedGUIState()
{
    switch(g_GUIController->state()) {
    case DEFAULT_MODE:
        m_textColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_borderColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_bgColor.set(1.0f, 1.0f, 1.0f, 0.25f);
        break;

    case CAMERA_MODE:
        m_textColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_borderColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_bgColor.set(1.0f, 0.8f, 0.2f, 0.5f);
        break;

    case EDIT_MODE:
        m_textColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_borderColor.set(1.0f, 1.0f, 1.0f, 1.0f);
        m_bgColor.set(0.2f, 0.1f, 0.9f, 0.5f);
        break;
    }
}


// ----------------------------------------------------------------------------
// Rendering
// ----------------------------------------------------------------------------

void OnscreenDisplay::drawCounters(double elapsed)
{
    const unsigned int margin = 10;
    float ty = m_normalFont->lineHeight();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    print(m_normalFont, margin, margin + ty, "FPS: %d", m_currentFPS);
    print(m_normalFont, margin, margin + ty*2, "SPS: %3.2f", m_currentSPS);
}

void OnscreenDisplay::drawObjectInfo(double elapsed)
{
    // calculate outline
    const unsigned int margin = 10;
    unsigned int tx = (unsigned int)m_normalFont->width("Object Info");
    unsigned int ty = (unsigned int)m_normalFont->lineHeight();

    // draw the polygon
    glColor4f(m_bgColor.x(), m_bgColor.y(), m_bgColor.z(), m_bgColor.w());
    glBegin(GL_POLYGON);
    glVertex2i(0, m_screenHeight - ty - margin*2);
    glVertex2i(tx + margin, m_screenHeight - ty - margin*2);
    glVertex2i(tx + margin + 36, m_screenHeight);
    glVertex2i(0, m_screenHeight);
    glEnd();

    // draw the outline
    glLineWidth(1.0f);
    glColor4f(m_borderColor.x(), m_borderColor.y(), m_borderColor.z(), m_borderColor.w());
    glBegin(GL_LINE_STRIP);
    glVertex2i(0, m_screenHeight - ty - margin*2);
    glVertex2i(tx + margin, m_screenHeight - ty - margin*2);
    glVertex2i(tx + margin + 36, m_screenHeight);
    glEnd();

    // draw the text
    glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
    print(m_normalFont, margin, m_screenHeight - margin, "Object Info");

    // draw the object info
    if (m_showObjectInfo) {
        unsigned int tx2 = (unsigned int)m_smallFont->width(m_objectInfoTitle);
        unsigned int ty2 = (unsigned int)m_smallFont->lineHeight();

        // draw the line
        unsigned int lx = 42;
        unsigned int ly = m_screenHeight - ty - margin*2;
        glColor4f(m_borderColor.x(), m_borderColor.y(), m_borderColor.z(), m_borderColor.w());
        glBegin(GL_LINE_STRIP);
        glVertex2i(lx, ly);
        glVertex2i(lx, ly - ty2*3);
        glVertex2i(lx + 30, ly - ty2 * 4 - 5);
        glVertex2i(lx + 30 + tx2, ly - ty2 * 4 - 5);
        glEnd();

        // draw the text
        //glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        print(m_smallFont, lx + 30, ly - ty2 * 4 - 10, m_objectInfoTitle.c_str());
        print(m_smallFont, lx + 30, ly - ty2 * 3, m_objectInfo1.c_str());
        print(m_smallFont, lx + 30, ly - ty2 * 2, m_objectInfo2.c_str());
        print(m_smallFont, lx + 30, ly - ty2 * 1, m_objectInfo3.c_str());
    }
}

void OnscreenDisplay::drawInputInfo(double elapsed)
{
    // calculate outline (based on the object info corner)
    const unsigned int margin = 10;
    unsigned int tx = (unsigned int)m_normalFont->width("Input Info");
    unsigned int ty = (unsigned int)m_normalFont->lineHeight();

    // draw the polygon
    glColor4f(m_bgColor.x(), m_bgColor.y(), m_bgColor.z(), m_bgColor.w());
    glBegin(GL_POLYGON);
    glVertex2i(m_screenWidth - tx - margin, m_screenHeight - ty - margin*2);
    glVertex2i(m_screenWidth, m_screenHeight - ty - margin*2);
    glVertex2i(m_screenWidth, m_screenHeight);
    glVertex2i(m_screenWidth - tx - margin - 36, m_screenHeight);
    glEnd();

    // draw the outline
    glLineWidth(1.0f);
    glColor4f(m_borderColor.x(), m_borderColor.y(), m_borderColor.z(), m_borderColor.w());
    glBegin(GL_LINE_STRIP);
    glVertex2i(m_screenWidth - tx - margin - 36, m_screenHeight);
    glVertex2i(m_screenWidth - tx - margin, m_screenHeight - ty - margin*2);
    glVertex2i(m_screenWidth, m_screenHeight - ty - margin*2);
    glEnd();

    // draw the text
    glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
    print(m_normalFont, m_screenWidth - tx - margin, m_screenHeight - margin, "Input Info");
    
    // draw the icon
    glPushMatrix();
    glTranslatef(m_screenWidth - margin, m_screenHeight - ty - margin*3.0f, 0.0f);
    // (0,0) is now the bottom right corner of the icon
    g_InputManager->primaryInputDevice()->draw();
    glPopMatrix();
}

void OnscreenDisplay::drawGUIState(double elapsed)
{
    const unsigned int margin = 10;
    unsigned int tx = (unsigned int)m_normalFont->width("Default Mode");
    unsigned int ty = (unsigned int)m_normalFont->lineHeight();
    unsigned int cx = m_screenWidth / 2;

    // draw the text
    char* text;
    switch(g_GUIController->state()) {
        case DEFAULT_MODE: text = "Default Mode"; break;
        case EDIT_MODE: text = "Edit Mode"; break;
        case CAMERA_MODE: text = "Camera Mode"; break;
    }

    tx = (unsigned int)m_normalFont->width(text);
    glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
    print(m_normalFont, cx - tx/2, margin + ty, text);
}

void OnscreenDisplay::drawServerStatus(double elapsed)
{
    // there is a copy of most of the constants in handleClick
    const unsigned int margin = 10;
    const unsigned int textMargin = 5;
    unsigned int tx = (unsigned int)m_normalFont->width("Simulation is running (0000000 steps)");
    unsigned int ty = (unsigned int)m_normalFont->lineHeight();
    unsigned int tdy = (unsigned int)(ty/2 + m_normalFont->descent());
    unsigned int iconSize = 18;
    unsigned int right = m_screenWidth - margin;
    unsigned int left = right - 2*margin - textMargin - tx - iconSize;
    unsigned int top = margin;
    unsigned int bottom = top + 2*textMargin + 2*ty;
    float progressScale = ((right - margin) - (left + margin)) / 100.0f;

    glLineWidth(1.0f);
    
    if (m_isSimulationRunning) {
        // Running-Icon
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex2i(left + margin, top + margin);
        glVertex2i(left + margin + iconSize, top + margin + iconSize/2);
        glVertex2i(left + margin, top + margin + iconSize);
        glEnd();
        // Running-Text
        glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
        print(m_normalFont, m_screenWidth - margin - textMargin - tx, top + textMargin + tdy, "Simulation is running (%d steps)", m_stepCount);
        print(m_smallFont, m_screenWidth - margin - textMargin - tx, top + textMargin + ty + tdy, "[click to pause]");
    } else {
        // Pause-Icon
        glColor4f(0.5f, 0.5f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2i(left + margin, top + margin);
        glVertex2i(left + margin + iconSize/3, top + margin);
        glVertex2i(left + margin + iconSize/3, top + margin + iconSize);
        glVertex2i(left + margin, top + margin + iconSize);
        glVertex2i(left + margin + 2*iconSize/3, top + margin);
        glVertex2i(left + margin + iconSize, top + margin);
        glVertex2i(left + margin + iconSize, top + margin + iconSize);
        glVertex2i(left + margin + 2*iconSize/3, top + margin + iconSize);
        glEnd();
        // Pause-Text
        glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
        print(m_normalFont, m_screenWidth - margin - textMargin - tx, top + textMargin + tdy, "Simulation paused (%d steps)", m_stepCount);
        print(m_smallFont, m_screenWidth - margin - textMargin - tx, top + textMargin + ty + tdy, "[click to start]");
    }

    if (m_showServerStatus && !m_serverStatus.empty()) {
        // Server Status-Message
        glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
        print(m_normalFont, left + margin, bottom + textMargin + tdy, m_serverStatus.c_str());
        // Server Progress
        if (m_serverProgress > 0)
        {
            glBegin(GL_QUADS);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(left + margin, bottom + 2*textMargin + ty);
            glVertex2f(left + margin, bottom + 2*textMargin + ty + 15);
            glVertex2f(left + margin + m_serverProgress*progressScale, bottom + 2*textMargin + ty + 15);
            glVertex2f(left + margin + m_serverProgress*progressScale, bottom + 2*textMargin + ty);
            glEnd();        
            glBegin(GL_LINE_LOOP);
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex2i(left + margin, bottom + 2*textMargin + ty);
            glVertex2i(left + margin, bottom + 2*textMargin + ty + 15);
            glVertex2i(right - margin, bottom + 2*textMargin + ty + 15);
            glVertex2i(right - margin, bottom + 2*textMargin + ty);
            glEnd();
        }
        // Border Line with server-Status
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glVertex2i(right, bottom + 2*textMargin + ty + margin + 15);
        glVertex2i(left, bottom + 2*textMargin + ty + margin + 15);
        glEnd();
    } else {
        // Border Line without server-Status
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glVertex2i(right, bottom);
        glVertex2i(left, bottom);
        glEnd();
    }
    
    return;


    if (!m_showServerStatus || m_serverStatus.empty()) return;

    glColor4f(m_textColor.x(), m_textColor.y(), m_textColor.z(), m_textColor.w());
    print(m_normalFont, m_screenWidth - margin - tx, margin + ty, m_serverStatus.c_str());
    
    if (m_serverProgress > 0)
    {
        glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2i(m_screenWidth - margin - 200 + 2*m_serverProgress, 2 * margin + ty);
        glVertex2i(m_screenWidth - margin - 200, 2 * margin + ty);
        glVertex2i(m_screenWidth - margin - 200, 2 * margin + ty + 15);
        glVertex2i(m_screenWidth - margin - 200 + 2*m_serverProgress, 2 * margin + ty + 15);
        glEnd();        
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2i(m_screenWidth - margin, 2 * margin + ty);
        glVertex2i(m_screenWidth - margin - 200, 2 * margin + ty);
        glVertex2i(m_screenWidth - margin - 200, 2 * margin + ty + 15);
        glVertex2i(m_screenWidth - margin, 2 * margin + ty + 15);
        glEnd();
    }
}

void OnscreenDisplay::drawSelection(double elapsed)
{
    if (!m_showSelection || !m_currentSelection)
        return;

    osgUtil::SceneView* sceneView = g_Application->viewer()->getSceneHandlerList().at(0)->getSceneView();
    osg::BoundingBox bbox = m_currentSelection->getBoundingBox();

    // calculate center
    osg::Vec3 center = bbox.center() * worldTransformForNode(m_currentSelection.get());
    osg::Vec3 projected_center;
    sceneView->projectObjectIntoWindow(center, projected_center);
    projected_center.y() = m_screenHeight - projected_center.y();

    // calculate radius in a stupidly expensive way
    float radius;
    osg::Vec3 c1, c2, c3, c4;
    sceneView->projectObjectIntoWindow(bbox.corner(0), c1);
    sceneView->projectObjectIntoWindow(bbox.corner(7), c2);
    sceneView->projectObjectIntoWindow(bbox.corner(1), c3);
    sceneView->projectObjectIntoWindow(bbox.corner(6), c4);

    radius = max((c1-c2).length(), (c3-c4).length()) / 2;

    glPushMatrix();
    glTranslatef(projected_center.x(), projected_center.y(), 0.0f);
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

    // draw left quarter-circle
    glBegin(GL_LINE_STRIP);
    for (int i = 135; i <= 225; i += 5) {
        float radians = i * 3.14159f / 180.0f;
        glVertex2f(cos(radians) * radius, sin(radians) * radius);
    }
    glEnd();

    // draw right quarter-circle
    glBegin(GL_LINE_STRIP);
    for (int i = 315; i <= 405; i += 5) {
        float radians = i * 3.14159f / 180.0f;
        glVertex2f(cos(radians) * radius, sin(radians) * radius);
    }
    glEnd();

    // draw the targeting lines
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 45; i <= 315; i+=90) {
        float radians = i * 3.14159f / 180.0f;
        glVertex2f(cos(radians) * (radius - 4), sin(radians) * (radius - 4));
        glVertex2f(cos(radians) * (radius + 8), sin(radians) * (radius + 8));
    }
    glEnd();

    float tx = m_smallFont->width(m_selectionName);
    float ty = m_smallFont->lineHeight();

    // draw the right text
    glBegin(GL_LINES);
    glVertex2f(radius, 0.0f);
    glVertex2f(radius + tx + 15, 0.0f);
    glEnd();

    print(m_smallFont, radius + 15, -5.0f, m_selectionName.c_str());

    // draw the left text

    if (m_currentDraggingMode != DRAGGINGMODE_NONE) {
      // Translation      
        tx = m_smallFont->width("Translate");
        if (m_currentDraggingMode == DRAGGINGMODE_TRANSLATE) {
            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        
            glBegin(GL_LINE_STRIP);
            glVertex2f(-radius - tx - 30, -ty);
            glVertex2f(-radius - 30, -ty);
            glVertex2f(-radius - 10, 0.0f);
            glVertex2f(-radius, 0.0f);
            glEnd();
        } else {
          glColor4f(0.0f, 0.6f, 0.0f, 1.0f);
        }
        print(m_smallFont, -radius - tx - 30, -ty - 5, "Translate");

        // Rotat    ion
        tx = m_smallFont->width("Rotate");
        if (m_currentDraggingMode == DRAGGINGMODE_ROTATE) {
          glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

          glBegin(GL_LINE_STRIP);
          glVertex2f(-radius - tx - 30, 5.0f);
          glVertex2f(-radius - 30, 5.0f);
          glVertex2f(-radius - 10, 0.0f);
          glVertex2f(-radius, 0.0f);
          glEnd();
        } else {
          glColor4f(0.0f, 0.6f, 0.0f, 1.0f);
        }
        print(m_smallFont, -radius - tx - 30, 0, "Rotate");

        // Scale    
        tx = m_smallFont->width("Scale");
        if (m_currentDraggingMode == DRAGGINGMODE_SCALE) {
          glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

          glBegin(GL_LINE_STRIP);
          glVertex2f(-radius - tx - 30, ty);
          glVertex2f(-radius - 30, ty);
          glVertex2f(-radius - 10, 0.0f);
          glVertex2f(-radius, 0.0f);
          glEnd();
        } else {
          glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        }
        print(m_smallFont, -radius - tx - 30, ty + 5, "Scale");
    }

    glPopMatrix();
}


void OnscreenDisplay::draw(double elapsed)
{
    m_lock.lock();

    // draw all on-screen elements
    drawCounters(elapsed);
    drawObjectInfo(elapsed);
    drawInputInfo(elapsed);
    drawGUIState(elapsed);
    drawServerStatus(elapsed);
    drawSelection(elapsed);

    // average the FPS over 60 frames
    m_averageFramesElapsed += elapsed;

    if (m_frameCount++ == 60) {
	setCurrentFPS((unsigned int)((1000 * m_frameCount) / m_averageFramesElapsed));
	m_averageFramesElapsed = 0.0;
	m_frameCount = 0;
    }

    // average steps per second every 10 steps
    m_averageStepsElapsed += elapsed;
    
    if ((m_stepCount - m_lastStepCount) >= 10) {
        setCurrentSPS((1000.0f * (m_stepCount - m_lastStepCount)) / m_averageStepsElapsed);
        m_averageStepsElapsed = 0.0;
        m_lastStepCount = m_stepCount;
    }

    m_lock.unlock();
}

bool OnscreenDisplay::handleClick(unsigned int mousePosX, unsigned int mousePosY)
{
    // copied from drawServerStatus
    const unsigned int margin = 10;
    const unsigned int textMargin = 5;
    unsigned int tx = (unsigned int)m_normalFont->width("Simulation is running (0000000 steps)");
    unsigned int ty = (unsigned int)m_normalFont->lineHeight();
    unsigned int iconSize = 18;
    unsigned int right = m_screenWidth - margin;
    unsigned int left = right - 2*margin - textMargin - tx - iconSize;
    unsigned int top = margin;
    unsigned int bottom = top + 2*textMargin + 2*ty;
    
    if ((mousePosX > left) && (mousePosX < right) && (mousePosY > top) && (mousePosY < bottom)) {
        if(modelConn != NULL) {
            if(!m_isSimulationRunning)
                modelConn->rpc("sim::start");
            else
                modelConn->rpc("sim::pause");
        }

        return true;
    }
    
    return false;
}
