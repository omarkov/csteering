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

#include <osg/ShapeDrawable>
#include <osg/PolygonMode>

#include "Application.h"
#include "SimulationSpace.h"
#include "NodeFlags.h"


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

SimulationSpace::SimulationSpace() : osg::Group()
{
    createGeometry();
}

SimulationSpace::SimulationSpace(const SimulationSpace& other, const osg::CopyOp& op)
    : osg::Group(other, op)
{
    m_dimensions = other.m_dimensions;

    createGeometry();
}


// ----------------------------------------------------------------------------
// Geometry creation
// ----------------------------------------------------------------------------

void SimulationSpace::createGeometry()
{
    m_geode = new osg::Geode();
    m_geode->setName("Simulation Space");
    m_geode->setNodeMask(DONT_IGNORE_FLAG);

    osg::Box* box = new osg::Box(osg::Vec3(), 1.0f, 1.0f, 1.0f);
    osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
    drawable->setColor(osg::Vec4(0.3f, 0.3f, 0.6f, 1.0f));

    osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
    pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

    osg::StateSet* ss = drawable->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(pm.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    m_geode->addDrawable(drawable);

    m_transform = new osg::MatrixTransform();
    m_transform->addChild(m_geode.get());
    addChild(m_transform.get());
}


// ----------------------------------------------------------------------------
// Dimensions
// ----------------------------------------------------------------------------

void SimulationSpace::setDimensions(float x, float y, float z)
{
    m_dimensions.set(x, y, z);

    osg::Vec3 center = g_Application->model()->getBound().center();

    m_transform->setMatrix(/*osg::Matrixd::translate(center) * */osg::Matrixd::scale(m_dimensions));
}

void SimulationSpace::dimensions(float& x, float& y, float& z) const
{
    x = m_dimensions.x();
    y = m_dimensions.y();
    z = m_dimensions.z();
}
