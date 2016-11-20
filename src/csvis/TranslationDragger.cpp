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

#include "TranslationDragger.h"
#include <osg/GL>
#include <osg/Drawable>
#include <osgProducer/Viewer>

#include "Application.h"
#include "GUIController.h"

const double DRAG_DELTA = 1.0f;

/*
// ----------------------------------------------------------------------------
// Callback responsible for drawing the circles
// ----------------------------------------------------------------------------

void TranslationDragger::CirclesCallback::drawImplementation(osg::State& state, const osg::Drawable* drawable) const
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glPopMatrix();
}
*/
// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

TranslationDragger::TranslationDragger()
    : Dragger()
{
    m_boundsWidth = 1.0f;
    m_boundsHeight = 1.0f;
    m_plane = PLANE_XY;
}

TranslationDragger::TranslationDragger(const TranslationDragger& other, const osg::CopyOp& op)
    : Dragger(other, op)
{
}


// ----------------------------------------------------------------------------
// Geometry creation
// ----------------------------------------------------------------------------

void TranslationDragger::createGeometry()
{
}


// ----------------------------------------------------------------------------
// Calculation
// ----------------------------------------------------------------------------

void TranslationDragger::updateBounds()
{
    unsigned int width, height;
    g_GUIController->screenSize(width, height);

    m_boundsWidth = 1.0f / (((float)width - 1.0f) * 0.5f);
    m_boundsHeight = 1.0f / (((float)height - 1.0f) * 0.5f);
}

void TranslationDragger::calculateFactor()
{
    // get all nescessary points to calculate the factor
    osgUtil::SceneView* sceneView = g_Application->viewer()->getSceneHandlerList().at(0)->getSceneView();
    osg::Vec3 near;
    osg::Vec3 far;
    sceneView->projectWindowXYIntoObject((int)(m_boundsWidth/2.0f), (int)(m_boundsHeight/2.0f), near, far);
    osg::Vec3 object = position();
    
    // calculate
    osg::Vec3 distance = near-object;
    m_factor = distance.length(); //sqrt(distance.x()*distance.x() + distance.y()*distance.y() + distance.z()*distance.z());    
}

osg::Vec3 TranslationDragger::mapToPlane(float x, float y)
{
    osg::Vec2 temp;

    temp.set((x * m_boundsWidth), (y * m_boundsHeight));
   
    osg::Vec3 vector;
    switch(m_plane) {
        case PLANE_NONE: vector.set(0.0f, 0.0f, 0.0f); break;
        case PLANE_XY:   vector.set(temp.x(), -temp.y(), 0.0f); break;
        case PLANE_XZ:   vector.set(temp.x(), 0.0f, temp.y()); break;
        case PLANE_YZ:   vector.set(0.0f, -temp.y(), temp.x()); break;
    }
    return vector;
}

void TranslationDragger::restrictToPlane(TranslationDraggerPlane plane)
{
    if (m_translation.length() > DRAG_DELTA) {
        // mouse has moved since the last stored matrix
        m_hasChanged = true;
        // store matrix
        osg::Matrixd newMatrix;
        computeLocalToWorldMatrix(newMatrix, NULL);
        m_lastMatrix = newMatrix;
        // reset current translation
        m_translation.set(0.0f, 0.0f, 0.0f);
        // store new mouse coordinates
        m_savedX = m_mouseOriginX;
        m_savedY = m_mouseOriginY;
    }
    // restrict to new plane
    m_plane = plane;
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

void TranslationDragger::handleButtonPressed(int button)
{
    if (g_GUIController->state() == CAMERA_MODE)
        return;
    switch(button) {
        case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
            beginDrag();
            break;
        case osgGA::GUIEventAdapter::KEY_Shift_L:
        case osgGA::GUIEventAdapter::KEY_Shift_R:
        case osgGA::GUIEventAdapter::KEY_Control_L:
        case osgGA::GUIEventAdapter::KEY_Control_R:
            restrictToPlane(PLANE_XZ);
            break;
        case osgGA::GUIEventAdapter::KEY_Alt_L:
        case osgGA::GUIEventAdapter::KEY_Alt_R:
            restrictToPlane(PLANE_YZ);
            break;
        default:
            break;
    }
}

void TranslationDragger::handleButtonReleased(int button)
{
    if (g_GUIController->state() == CAMERA_MODE)
        return;
    switch(button) {
        case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
            endDrag();
            break;
        case osgGA::GUIEventAdapter::KEY_Shift_L:
        case osgGA::GUIEventAdapter::KEY_Shift_R:
        case osgGA::GUIEventAdapter::KEY_Control_L:
        case osgGA::GUIEventAdapter::KEY_Control_R:
        case osgGA::GUIEventAdapter::KEY_Alt_L:
        case osgGA::GUIEventAdapter::KEY_Alt_R:
            restrictToPlane(PLANE_XY);
            break;
    }
}


// ----------------------------------------------------------------------------
// Drag handling
// ----------------------------------------------------------------------------

void TranslationDragger::handleDragBegin()
{
    updateBounds();
    calculateFactor();

    if (m_currentInputDevice->provides3DData())
        m_deviceOrigin = m_currentInputDevice->position();

    m_savedX = m_mouseOriginX;
    m_savedY = m_mouseOriginY;
    m_hasChanged = false;
}

void TranslationDragger::handleDrag()
{
    updateBounds();

    if (m_currentInputDevice->provides3DData()) {
      m_translation = (m_currentInputDevice->position() - m_deviceOrigin) * m_factor * m_boundsWidth;
    } else {
      m_translation = mapToPlane(m_mouseOriginX-m_savedX, m_mouseOriginY-m_savedY) * m_factor;
    }

    dirtyBound();
}

void TranslationDragger::handleDragEnd()
{
    if (m_translation.length() > DRAG_DELTA) {
        m_hasChanged = true;
        osg::Matrixd newMatrix;
        computeLocalToWorldMatrix(newMatrix, NULL);
        m_lastMatrix = newMatrix;
    }
    m_translation.set(0.0f, 0.0f, 0.0f);
}

// ----------------------------------------------------------------------------
// Setter and Getter
// ----------------------------------------------------------------------------

void TranslationDragger::setPosition(osg::Vec3 position)
{
    m_lastMatrix.makeTranslate(position);
    if (m_delegate)
        m_delegate->draggerDidEndDragging(true);
}

osg::Vec3 TranslationDragger::position() const
{
    osg::Matrixd currentMatrix;
    computeLocalToWorldMatrix(currentMatrix, NULL);
    return osg::Vec3(0.0f, 0.0f, 0.0f) * currentMatrix;
}

// ----------------------------------------------------------------------------
// Matrix compututation
// ----------------------------------------------------------------------------

bool TranslationDragger::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    osg::Matrixd view = g_Application->viewer()->getViewMatrix();
    osg::Vec3 center, eye, up;
    view.getLookAt(center, eye, up);

    if (_referenceFrame == RELATIVE_RF)
        matrix.preMult(m_lastMatrix *
                       view *
                       osg::Matrixd::translate(m_translation) *
                       osg::Matrixd::inverse(view));
    else
        matrix = (m_lastMatrix *
                  view *
                  osg::Matrixd::translate(m_translation) *
                  osg::Matrixd::inverse(view));

    return true;
}

bool TranslationDragger::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    osg::Matrixd view = g_Application->viewer()->getViewMatrix();
    osg::Vec3 center, eye, up;
    view.getLookAt(center, eye, up);

    computeBound();

    if (_referenceFrame == RELATIVE_RF)
        matrix.postMult(osg::Matrixd::inverse(m_lastMatrix) *
                        view *
                        osg::Matrixd::translate(-m_translation) *
                        osg::Matrixd::inverse(view));
    else
        matrix = (osg::Matrixd::inverse(m_lastMatrix) *
                  view *
                  osg::Matrixd::translate(-m_translation) *
                  osg::Matrixd::inverse(view));

    return true;
} 

