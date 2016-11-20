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

#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osgSim/ScalarsToColors>
#include <osgSim/ColorRange>
#include <osgSim/ScalarBar>

#include "Application.h"
#include "GUIController.h"
#include "NodeFlags.h"
#include "Probe.h"
#include "common.h"


// ----------------------------------------------------------------------------
// Update calllback
// ----------------------------------------------------------------------------

class ProbeUpdateCallback : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
    {
        Probe *probe = dynamic_cast<Probe*>(node);

        if (probe) probe->update();

        traverse(node, nv);
    }
};


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

/**
 * The constructor allocates all standard OSG nodes for a probe.
 *
 * @param id a unique ID identifying the probe
 */
Probe::Probe(unsigned int id) : m_id(id), m_colormap(0), m_sampleType(SAMPLETYPE_NONE)
{
    m_customMinMax = false;
    m_colorRangeMin = 0.0;
    m_colorRangeMax = 0.0;
    m_lastMinimum = 0.0;
    m_lastMaximum = 1.0;

    // allocate all nodes
    m_emitterGeometry = new osg::Geode();
    m_emitterGeometry->setNodeMask(CLICKABLE_FLAG);

    m_selectionSwitch = new osg::Switch();

    m_dragger = new TransformDragger();
    m_dragger->setDelegate(this);

    addChild(m_dragger.get());
    m_dragger->addChild(m_selectionSwitch.get());

    m_selectionSwitch->setAllChildrenOff();

    // set callback to update the probe data
    setUpdateCallback(new ProbeUpdateCallback);
}

Probe::~Probe()
{
}


void Probe::assignIDToName()
{
    char name[10];
    snprintf(name, sizeof(name), "Probe %d", m_id);

    m_emitterGeometry->setName(name);
    setName(name);
}


// ----------------------------------------------------------------------------
// Dragger delegate
// ----------------------------------------------------------------------------

void Probe::draggerDidMove()
{
    //notifySimulation();
    printPositionToObjectInfo(m_emitterGeometry->getName(), m_dragger->position());
}

void Probe::draggerDidEndDragging(bool hasChanged)
{
    if (hasChanged)
        notifySimulation();
}


// ----------------------------------------------------------------------------
// Positioning
// ----------------------------------------------------------------------------

/**
 * @param pos the position in OSG space
 */
void Probe::setPosition(const osg::Vec3& pos)
{
    m_dragger->setPosition(pos);
}

osg::Vec3 Probe::position() const
{
    return m_dragger->position();
}

/**
 * @param quat a quaternion in OSG space
 */
void Probe::setAttitude(const osg::Quat& quat)
{
    m_dragger->setRotation(quat);
}

osg::Quat Probe::attitude() const
{
    return m_dragger->rotation();
}


// ----------------------------------------------------------------------------
// Selection
// ----------------------------------------------------------------------------

void Probe::handleSelected()
{
    m_dragger->enable();
    m_selectionSwitch->setAllChildrenOn();

    g_GUIController->osd()->showSelectionReticule(m_emitterGeometry, m_emitterGeometry->getName());
    printPositionToObjectInfo(m_emitterGeometry->getName(), m_dragger->position());
}

void Probe::handleDeselected()
{
    m_dragger->disable();
    m_selectionSwitch->setAllChildrenOff();

    g_GUIController->osd()->hideObjectInfo();
    g_GUIController->osd()->hideSelectionReticule();
}


// ----------------------------------------------------------------------------
// Color range
// ----------------------------------------------------------------------------

/**
 * Set the color range to use for visualizing the results.
 *
 * @param cr a color range
 */
void Probe::setColorRange(const std::vector<osg::Vec4>& colors, float min, float max)
{
    osgSim::ColorRange* cr = new osgSim::ColorRange(min, max, colors);

    // this is kind of dirty
    if (m_scalarBar.valid())
        m_selectionSwitch->removeChild(m_scalarBar->getParent(0)->getParent(0));

    m_scalarBar = new osgSim::ScalarBar(128, 2, cr, m_emitterGeometry->getName(),
                                        osgSim::ScalarBar::HORIZONTAL, 0.035f);

    // use our default font
    osgSim::ScalarBar::TextProperties tp;
    tp._fontFile = g_Application->dataPath("fonts/andalemo.ttf");
    tp._characterSize = 0.07f;
    m_scalarBar->setTextProperties(tp);

    // disable lighting and depth testing
    osg::StateSet* ss = m_scalarBar->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    osg::MatrixTransform* modelview = new osg::MatrixTransform;
    osg::Matrixd matrix(osg::Matrixd::scale(500, 500, 500) * osg::Matrixd::translate(280, 20, 0));

    modelview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modelview->setMatrix(matrix);
    modelview->addChild(m_scalarBar.get());

    // set up the orthographic projection
    unsigned int width, height;
    g_GUIController->screenSize(width, height);

    osg::Projection* projection = new osg::Projection;
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 1024));
    projection->addChild(modelview);

    m_selectionSwitch->addChild(projection);

    m_colorRange = colors;

    // update the contents
}

void Probe::setColorRangeIndex(unsigned int index, float min, float max)
{
    if (index > 5)
        return;

    m_colormap = index;

    std::vector<osg::Vec4> colors;
    switch(index)
    {
        case 0: // Temperature
            colors.push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f)); // black
            colors.push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // red
            colors.push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)); // yellow
            colors.push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); // white
            break;
        case 1: // Magenta
            colors.push_back(osg::Vec4(0.13f, 0.1f, 0.09f, 1.0f));
            colors.push_back(osg::Vec4(0.440f, 0.33f, 0.29f, 1.0f));
            colors.push_back(osg::Vec4(0.87f, 0.24f, 0.11f, 1.0f));
            colors.push_back(osg::Vec4(0.87f, 0.25f, 0.46f, 1.0f));
            colors.push_back(osg::Vec4(0.57f, 0.34f, 0.58f, 1.0f));
            colors.push_back(osg::Vec4(0.62f, 0.43f, 0.64f, 1.0f));
            colors.push_back(osg::Vec4(0.83f, 0.72f, 0.84f, 1.0f));
            colors.push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            break;
        case 2: // Rainbow
            colors.push_back(osg::Vec4(0.31f, 0.24f, 0.52f, 1.0f));
            colors.push_back(osg::Vec4(0.38f, 0.73f, 0.76f, 1.0f));
            colors.push_back(osg::Vec4(0.32f, 0.72f, 0.67f, 1.0f));
            colors.push_back(osg::Vec4(0.21f, 0.68f, 0.52f, 1.0f));
            colors.push_back(osg::Vec4(0.0f, 0.65f, 0.35f, 1.0f));
            colors.push_back(osg::Vec4(0.95f, 0.93f, 0.0f, 1.0f));
            colors.push_back(osg::Vec4(0.91f, 0.53f, 0.16f, 1.0f));
            colors.push_back(osg::Vec4(0.86f, 0.24f, 0.18f, 1.0f));
            colors.push_back(osg::Vec4(0.88f, 0.24f, 0.51f, 1.0f));
            break;
        case 3: // Blue-Red
            colors.push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
            colors.push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
        case 4: // Blue-White
            colors.push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
            colors.push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            break;
        case 5: // Green-Yellow-Red
            colors.push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
            colors.push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
            colors.push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
    }
    setColorRange(colors, 0, 1);
}

// ----------------------------------------------------------------------------
// Sample type
// ----------------------------------------------------------------------------

void Probe::setSampleType(sample_type type)
{
    m_sampleType = type;
    notifySimulation();
}
