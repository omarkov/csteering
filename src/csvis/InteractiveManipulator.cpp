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

#include "InputManager.h"
#include "DTrackInputDevice.h"
#include "InteractiveManipulator.h"
#include "GUIController.h"
#include "Application.h"

// ---------------------------------------------------------------------------
// Construction/Deconstruction
// ---------------------------------------------------------------------------

InteractiveManipulator::InteractiveManipulator()
: m_isButtonDown(false)
{
}

InteractiveManipulator::~InteractiveManipulator()
{
}


void InteractiveManipulator::updateBounds()
{
    unsigned int width, height;
    g_GUIController->screenSize(width, height);

    m_boundsWidth = 1.0f / (((float)width - 1.0f) * 0.5f);
    m_boundsHeight = 1.0f / (((float)height - 1.0f) * 0.5f);
}

// ---------------------------------------------------------------------------
// Matrix getters & setters
// ---------------------------------------------------------------------------

void InteractiveManipulator::setByMatrix(const osg::Matrixd& matrix)
{
    m_matrix = matrix;
}

void InteractiveManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
    m_matrix = osg::Matrixd::inverse(matrix);
}

osg::Matrixd InteractiveManipulator::getMatrix() const
{
    InputDevice* device = g_InputManager->primaryInputDevice();

    if (device && m_isButtonDown)
      return osg::Matrixd::translate(device->position()*g_Application->rootNode()->getBound().radius()*m_boundsWidth)
        * osg::Matrixd::inverse(osg::Matrixd::translate(m_position*g_Application->rootNode()->getBound().radius()*m_boundsWidth))
        * osg::Matrixd::rotate(m_rotation)
        * osg::Matrixd::inverse(osg::Matrixd::rotate(device->rotation()))
        * m_matrix;
    else
      return m_matrix;
}

osg::Matrixd InteractiveManipulator::getInverseMatrix() const
{
  return osg::Matrixd::inverse(getMatrix());
}

// ---------------------------------------------------------------------------
// Origin node getters & setters
// ---------------------------------------------------------------------------

void InteractiveManipulator::setNode(osg::Node* node)
{
    if (!node)
        return;
        
    m_node = node;
    
    if (m_node.get()) {
        const osg::BoundingSphere& sphere = m_node->getBound();
        
        setByMatrix(osg::Matrix::translate(0.0f, 0.0f, 0.0f * sphere._radius) *
                    osg::Matrixf::rotate(0.9f, 1.0f, 0.0f, 0.0f));
    }
}

osg::Node* InteractiveManipulator::getNode()
{
    return m_node.get();
}

const osg::Node* InteractiveManipulator::getNode() const
{
    return m_node.get();
}


// ---------------------------------------------------------------------------
// GUI event handler
// ---------------------------------------------------------------------------

bool InteractiveManipulator::handle(const osgGA::GUIEventAdapter& eventAdapter,
                                osgGA::GUIActionAdapter& actionAdapter)
{
    DTrackInputDevice* device = dynamic_cast<DTrackInputDevice*>(g_InputManager->primaryInputDevice());

    if (!device || !device->provides3DData())
        return false;

  
    if (eventAdapter.getEventType() == osgGA::GUIEventAdapter::FRAME) {
        // was the button pressed?
        if (!m_isButtonDown && device->isButtonPressed(FLY_MIDDLE_BUTTON)) {
            m_isButtonDown = true;
            g_GUIController->setState(CAMERA_MODE);
            updateBounds();
            m_position = device->position();
            m_rotation = device->rotation();
        // was the button releaseed?
        } else if (m_isButtonDown && !device->isButtonPressed(FLY_MIDDLE_BUTTON)) {
            m_isButtonDown = false;
            g_GUIController->setPreviousState();
            m_matrix = osg::Matrixd::translate(device->position()*g_Application->rootNode()->getBound().radius()*m_boundsWidth)
             * osg::Matrixd::inverse(osg::Matrixd::translate(m_position*g_Application->rootNode()->getBound().radius()*m_boundsWidth))
             * osg::Matrixd::rotate(m_rotation)
             * osg::Matrixd::inverse(osg::Matrixd::rotate(device->rotation()))
             * m_matrix;
        }
    }

    return false;
}
