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

#include "Application.h"
#include "FANClasses.h"
#include "CommonServer.h"
#include "GlyphProbe.h"
#include "common.h"


// ----------------------------------------------------------------------------
// Construction / Destruction
// ----------------------------------------------------------------------------

GlyphProbe::GlyphProbe(unsigned int id) : Probe(id)
{
    m_numStreams = 0;
    m_lastSample = NULL;

    // get scaling factors for Z
    double fx, fy, fz;
    g_Application->model()->scalingFactors(fx, fy, fz);
    m_emitterHeight = 1.0f * fz;

    m_dragger->addChild(m_emitterGeometry.get());
    createEmitterGeometry();

    m_glyphGeode = new osg::Geode();
    m_glyphGeode->setName(m_emitterGeometry->getName());
    m_dragger->addChild(m_glyphGeode.get());
}

// FIXME: doesn't work
GlyphProbe::GlyphProbe(const GlyphProbe& other, const osg::CopyOp& op) : Probe(other.m_id)
{
    m_numStreams = other.m_numStreams;
    m_lastSample = NULL;

    m_emitterHeight = other.m_emitterHeight;
}

GlyphProbe::~GlyphProbe()
{
}

void GlyphProbe::createEmitterGeometry()
{
   // get scaling factors for X and Y
   double fx, fy, fz;
   g_Application->model()->scalingFactors(fx, fy, fz);

   osg::Vec3 origin(0.0f, 0.0f, 0.0f);
   osg::Vec3 size(0.02f * fz, 0.02f * fz, m_emitterHeight);

   osg::Box* box = new osg::Box(origin, size.x(), size.y(), size.z());
   osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);

   m_emitterGeometry->removeDrawable(0, m_emitterGeometry->getNumDrawables());
   m_emitterGeometry->addDrawable(drawable);

   assignIDToName();
}


// ----------------------------------------------------------------------------
// Emitter and stream parameters
// ----------------------------------------------------------------------------

void GlyphProbe::setEmitterHeight(float height)
{
    m_emitterHeight = height;
    createEmitterGeometry();
}

void GlyphProbe::setNumStreams(unsigned int num)
{
    m_numStreams = num;
}


// ----------------------------------------------------------------------------
// Communication to the simulation
// ----------------------------------------------------------------------------

void GlyphProbe::notifySimulation()
{
    std::vector<osg::Vec3> array;
    osg::Matrixd M;

    m_dragger->computeLocalToWorldMatrix(M, NULL);

    for (unsigned int i = 0; i < m_numStreams; i++) {
        float offset = i * (m_emitterHeight / (float)(m_numStreams - 1));

	array.push_back(osg::Vec3(0.0f, 0.0f, offset)*M);
    }

    vertex_t *vec = new vertex_t[array.size()];
    for(unsigned int i = 0; i < array.size(); i++)
    {
        vec2vertex(array[i] , &vec[i]);
    }

    if(modelConn != NULL) {
        modelConn->startBinaryPush("sim::newSample");
        char *templ;
        asprintf(&templ, "int;int;int;int;{double;double;double}[%d]", array.size());
        modelConn->binaryPush(templ, id(), m_sampleType, PROBETYPE_GLYPH, array.size(), vec);
        modelConn->stopBinaryPush();
        free(templ);
    }else
    {
        FAN_postMessage(masterCom, "login", NULL, NULL);
    }


    delete[] vec;
}


// ----------------------------------------------------------------------------
// Communication from the the simulation
// ----------------------------------------------------------------------------

void GlyphProbe::setData(glyph_probe_data *data)
{
    m_updateLock.lock();

    if (m_lastSample) {
        zap_array(m_lastSample->positions);
        zap_array(m_lastSample->directions);
        zap_array(m_lastSample->values);
        zap(m_lastSample);
    }

    m_lastSample = data;

    m_updateLock.unlock();
}

void GlyphProbe::update()
{
    // do we have anything to update?
    if (!m_lastSample)
        return;

    m_updateLock.lock();

    // remove all drawables from the glyph geode
    m_glyphGeode->removeDrawable(0, m_glyphGeode->getNumDrawables());

    // recreate the scalarbar if min/max changed
    const osgSim::ScalarsToColors* currentSTC = m_scalarBar->getScalarsToColors();
    if (currentSTC->getMin() != m_lastSample->min_value || currentSTC->getMax() != m_lastSample->max_value) {
        m_scalarBar->setScalarsToColors(new osgSim::ColorRange(m_lastSample->min_value, m_lastSample->max_value, m_colorRange));
        currentSTC = m_scalarBar->getScalarsToColors();
    }

    int index = 0;

    // one stream a time
    for (unsigned int i = 0; i < m_lastSample->num_streams; i++) {
        int num_glyphs = m_lastSample->num_glyphs_per_stream[i];
        for (int j = 0; j < num_glyphs; j++) {
            // extract the color
            osg::Vec4 color = currentSTC->getColor(m_lastSample->values[index]);

            // calculate the position and rotation
            osg::Vec3 position(m_lastSample->positions[index].x, m_lastSample->positions[index].y, m_lastSample->positions[index].z);
            osg::Vec3 direction(m_lastSample->directions[index].x, m_lastSample->directions[index].x, m_lastSample->directions[index].z);

            osg::Quat rotation;
            rotation.makeRotate(position, direction);

            // create the cone part of the arrow
            osg::Cone* cone = new osg::Cone(osg::Vec3(), 0.3f, 0.6f);
            cone->setCenter(position); // FIXME
            cone->setRotation(rotation);
            osg::ShapeDrawable* coneDrawable = new osg::ShapeDrawable(cone);
            coneDrawable->setColor(color);

            // create the cylinder part
            osg::Cylinder* cylinder = new osg::Cylinder(osg::Vec3(), 0.1f, 0.6f);
            cylinder->setCenter(position);
            cylinder->setRotation(rotation);
            osg::ShapeDrawable* cylinderDrawable = new osg::ShapeDrawable(cylinder);
            cylinderDrawable->setColor(color);

            m_glyphGeode->addDrawable(coneDrawable);
            m_glyphGeode->addDrawable(cylinderDrawable);

            // calculate the linearized index
            index ++;
        }
    }

    zap_array(m_lastSample->positions);
    zap_array(m_lastSample->directions);
    zap_array(m_lastSample->values);
    zap(m_lastSample);

    m_updateLock.unlock();
}
