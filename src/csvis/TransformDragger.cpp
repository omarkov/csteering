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

#include "TransformDragger.h"
#include <osg/GL>
#include <osg/Drawable>
#include <osgProducer/Viewer>

#include "Application.h"
#include "InputManager.h"

#include "GUIController.h"
#include "OnscreenDisplay.h"

// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

TransformDragger::TransformDragger()
    : Dragger()
{
    m_translationDragger = new TranslationDragger();
    m_rotationDragger = new RotationDragger();
    m_translationDragger->setDelegate(this);
    m_rotationDragger->setDelegate(this);

    m_activeDragger = m_translationDragger;
    g_GUIController->osd()->setDraggingMode(DRAGGINGMODE_TRANSLATE);
}

TransformDragger::TransformDragger(const TransformDragger& other, const osg::CopyOp& op)
    : Dragger(other, op)
{
}

TransformDragger::~TransformDragger()
{
    delete m_translationDragger;
    delete m_rotationDragger;
}

// ----------------------------------------------------------------------------
// Geometry creation
// ----------------------------------------------------------------------------

void TransformDragger::createGeometry()
{
    m_translationDragger->createGeometry();
    m_rotationDragger->createGeometry();
}


// ----------------------------------------------------------------------------
// Calculation
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

void TransformDragger::handleButtonPressed(int button)
{
    if (g_InputManager->primaryInputDevice()->provides3DData()) {
        m_translationDragger->handleButtonPressed(button);
        m_rotationDragger->handleButtonPressed(button);
    } else
        m_activeDragger->handleButtonPressed(button);
}

void TransformDragger::handleButtonReleased(int button)
{
    if (g_InputManager->primaryInputDevice()->provides3DData()) {
        m_translationDragger->handleButtonReleased(button);
        m_rotationDragger->handleButtonReleased(button);
    } else
        m_activeDragger->handleButtonReleased(button);
}

// ----------------------------------------------------------------------------
// Drag handling
// ----------------------------------------------------------------------------

void TransformDragger::drag()
{
    if (g_InputManager->primaryInputDevice()->provides3DData()) {
        m_translationDragger->drag();
        m_rotationDragger->drag();
    } else
        m_activeDragger->drag();

    if (g_InputManager->primaryInputDevice()->provides3DData())
        g_GUIController->osd()->setDraggingMode(DRAGGINGMODE_NONE);

    dirtyBound();
}

void TransformDragger::setNextDraggerActive()
{
    if (m_activeDragger == m_translationDragger) {
        m_activeDragger = m_rotationDragger;
        g_GUIController->osd()->setDraggingMode(DRAGGINGMODE_ROTATE);
    } else {
        m_activeDragger = m_translationDragger;
        g_GUIController->osd()->setDraggingMode(DRAGGINGMODE_TRANSLATE);
    }

    if (g_InputManager->primaryInputDevice()->provides3DData())
        g_GUIController->osd()->setDraggingMode(DRAGGINGMODE_NONE);
}
    
// ----------------------------------------------------------------------------
// Setter and Getter
// ----------------------------------------------------------------------------

void TransformDragger::setPosition(osg::Vec3 position)
{
    m_translationDragger->setPosition(position);
}

osg::Vec3 TransformDragger::position() const
{
    return m_translationDragger->position();
}

void TransformDragger::setRotation(osg::Quat rotation)
{
    m_rotationDragger->setRotation(rotation);
}

osg::Quat TransformDragger::rotation() const
{
    return m_rotationDragger->rotation();
}

// ----------------------------------------------------------------------------
// Matrix compututation
// ----------------------------------------------------------------------------

bool TransformDragger::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    osg::Matrixd translation;
    osg::Matrixd rotation;
    m_translationDragger->computeLocalToWorldMatrix(translation, NULL);
    m_rotationDragger->computeLocalToWorldMatrix(rotation, NULL);
 
    if (_referenceFrame == RELATIVE_RF)
        matrix.preMult(rotation * translation);
    else
        matrix = (rotation * translation);

    return true;
}

bool TransformDragger::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor*) const
{
    m_translationDragger->computeWorldToLocalMatrix(matrix, NULL);
    m_rotationDragger->computeWorldToLocalMatrix(matrix, NULL);
    return true;
} 

// ----------------------------------------------------------------------------
// Dragger Delegate
// ----------------------------------------------------------------------------

void TransformDragger::draggerDidBeginDragging()
{
    if (m_delegate)
        m_delegate->draggerDidBeginDragging();
}

void TransformDragger::draggerDidMove()
{
    if (m_delegate)
        m_delegate->draggerDidMove();
}

void TransformDragger::draggerDidEndDragging(bool hasChanged)
{
    // FIXME: all the delegate-functions are called twice when dragging with 3DData

    if (m_delegate)
        m_delegate->draggerDidEndDragging(hasChanged);

    if (!hasChanged)
        setNextDraggerActive();
}
