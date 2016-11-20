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

#include "RotationDragger.h"

#include "Application.h"
#include "GUIController.h"

const double DRAG_DELTA = 0.01f;

/*
// ----------------------------------------------------------------------------
// Callback responsible for drawing the circles
// ----------------------------------------------------------------------------

void RotationDragger::CirclesCallback::drawImplementation(osg::State& state, const osg::Drawable* drawable) const
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glPopMatrix();
}
*/
// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

RotationDragger::RotationDragger()
    : Dragger()
{
    m_boundsWidth = 1.0f;
    m_boundsHeight = 1.0f;
}

RotationDragger::RotationDragger(const RotationDragger& other, const osg::CopyOp& op)
    : Dragger(other, op)
{
}


// ----------------------------------------------------------------------------
// Geometry creation
// ----------------------------------------------------------------------------

void RotationDragger::createGeometry()
{
}


// ----------------------------------------------------------------------------
// Calculation
// ----------------------------------------------------------------------------

void RotationDragger::updateBounds()
{
    unsigned int width, height;
    g_GUIController->screenSize(width, height);

    m_boundsWidth = 1.0f / (((float)width - 1.0f) * 0.5f);
    m_boundsHeight = 1.0f / (((float)height - 1.0f) * 0.5f);
}

void RotationDragger::mapMousePosToSphere(osg::Vec3& vector) const
{
    osg::Vec2 temp;
    float length;

    // adjust point coordinates and scale down to range [-1, 1]
    temp.set((m_mouseOriginX * m_boundsWidth) - 1.0f,
             1.0f - (m_mouseOriginY * m_boundsHeight));

    // compute the squarte of the length of the vector to the point from the center
    length = (temp.x() * temp.x()) + (temp.y() * temp.y());

    // where's the point?
    if (length > 1.0f) {
        // outside the sphere
        float norm = 1.0f / sqrt(length);

        // return the "normalized" vector, a point on the sphere
        vector.set(temp.x() * norm, temp.y() * norm, 0.0f);
    } else {
        // inside the sphere
        vector.set(temp.x(), temp.y(), sqrt(1.0f - length));
    }
}


// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

void RotationDragger::handleButtonPressed(int button)
{
    if (g_GUIController->state() == CAMERA_MODE)
        return;

    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        beginDrag();
}

void RotationDragger::handleButtonReleased(int button)
{
    if (g_GUIController->state() == CAMERA_MODE)
        return;

    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        endDrag();
}


// ----------------------------------------------------------------------------
// Drag handling
// ----------------------------------------------------------------------------

void RotationDragger::handleDragBegin()
{
    updateBounds();

    if (m_currentInputDevice->provides3DData())
      m_savedRotation = m_currentInputDevice->rotation().inverse();
    else
      mapMousePosToSphere(m_savedClick);

    m_hasChanged = false;
}

void RotationDragger::handleDrag()
{
    updateBounds();

    if (m_currentInputDevice->provides3DData()) {
      m_rotation = m_savedRotation.inverse() * m_currentInputDevice->rotation().inverse();
    } else {
      mapMousePosToSphere(m_savedDrag);

      // compute   the vector perpendicular to the begin and end vectors
      osg::Vec3 perpendicular = m_savedClick ^ m_savedDrag;

      // compute the len  gth of the perpendicular vector
      if (perpendicular.length() > 1.0e-5) {
        m_rotation.set(perpendicular.x(),
                       perpendicular.y(),
                       perpendicular.z(),
                       m_savedClick * m_savedDrag);
      }  else {
        m_rotation.set(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }

    dirtyBound();
}

void RotationDragger::handleDragEnd()
{
    if (m_rotation.length() > DRAG_DELTA) {
        m_hasChanged = true;
        osg::Matrixd newMatrix;
        computeLocalToWorldMatrix(newMatrix, NULL);
        m_lastMatrix = newMatrix;
    }
    m_rotation.set(0.0f, 0.0f, 0.0f, 0.0f);
}

// ----------------------------------------------------------------------------
// Setter and Getter
// ----------------------------------------------------------------------------

void RotationDragger::setRotation(osg::Quat rotation)
{
    rotation.get(m_lastMatrix);
    if (m_delegate)
        m_delegate->draggerDidEndDragging(true);
}

osg::Quat RotationDragger::rotation() const
{
    osg::Quat rotation;
    rotation.set(m_lastMatrix);
    return rotation;
}

// ----------------------------------------------------------------------------
// Matrix compututation
// ----------------------------------------------------------------------------

bool RotationDragger::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    osg::Matrixd view = g_Application->viewer()->getViewMatrix();
    osg::Vec3 center, eye, up;
    view.getLookAt(center, eye, up);
    if (_referenceFrame == RELATIVE_RF)
        matrix.preMult(m_lastMatrix * 
                       osg::Matrixd::translate(center) *
                       view *
                       osg::Matrixd::rotate(m_rotation) *
                       osg::Matrixd::inverse(view) *
                       osg::Matrixd::translate(-center));
    else
        matrix = (m_lastMatrix *
                  osg::Matrixd::translate(center) *
                  view *
                  osg::Matrixd::rotate(m_rotation) *
                  osg::Matrixd::inverse(view) *
                  osg::Matrixd::translate(-center));

    return true;
}

bool RotationDragger::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    osg::Matrixd view = g_Application->viewer()->getViewMatrix();
    osg::Vec3 center, eye, up;
    view.getLookAt(center, eye, up);

    if (_referenceFrame == RELATIVE_RF)
        matrix.postMult(osg::Matrixd::inverse(m_lastMatrix) *
                        osg::Matrixd::translate(center) *
                        view *
                        osg::Matrixd::rotate(m_rotation.inverse()) *
                        osg::Matrixd::inverse(view) *
                        osg::Matrixd::translate(-center));
    else
        matrix = (osg::Matrixd::inverse(m_lastMatrix) *
                  osg::Matrixd::translate(center) *
                  view *
                  osg::Matrixd::rotate(m_rotation.inverse()) *
                  osg::Matrixd::inverse(view) *
                  osg::Matrixd::translate(-center));

    return true;
} 

