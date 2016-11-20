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

#include <osg/Geometry>
#include <osg/ShapeDrawable>

#include "Application.h"
#include "FANClasses.h"
#include "CommonServer.h"
#include "NodeFlags.h"
#include "RibbonProbe.h"
#include "common.h"



// ----------------------------------------------------------------------------
// Construction / Destruction
// ----------------------------------------------------------------------------

RibbonProbe::RibbonProbe(unsigned int id) : Probe(id)
{
    m_numRibbons = 8;
    m_ribbonHeight = 25.0f;

    m_lastSample = NULL;

    // get scaling factors for Z
    double fx, fy, fz;
    g_Application->model()->scalingFactors(fx, fy, fz);
    m_emitterHeight = 1.0f * fz;

    m_dragger->addChild(m_emitterGeometry.get());
    createEmitterGeometry();

    m_ribbonGeode = new osg::Geode();
    m_ribbonGeode->setName(m_emitterGeometry->getName());
    m_ribbonGeode->setNodeMask(CLICKABLE_FLAG);

    m_dragger->addChild(m_ribbonGeode.get());
}

// FIXME: doesn't work
RibbonProbe::RibbonProbe(const RibbonProbe& other, const osg::CopyOp& op) : Probe(other.m_id)
{
    m_numRibbons = other.m_numRibbons;
    m_ribbonHeight = other.m_ribbonHeight;
    m_emitterHeight = other.m_emitterHeight;

    m_lastSample = 0;
}

RibbonProbe::~RibbonProbe()
{
    if (m_lastSample) {
        zap_array(m_lastSample->num_vertices);
        zap_array(m_lastSample->ribbons);
        zap_array(m_lastSample->values);
        zap(m_lastSample);
    }
}


// ----------------------------------------------------------------------------
// Geometry creation & change
// ----------------------------------------------------------------------------

void RibbonProbe::createEmitterGeometry()
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
// Emitter and ribbon parameters
// ----------------------------------------------------------------------------

void RibbonProbe::setEmitterHeight(float height)
{
    m_emitterHeight = height;
    createEmitterGeometry();
}

void RibbonProbe::setNumRibbons(unsigned int num)
{
    m_numRibbons = num;
}

void RibbonProbe::setRibbonHeight(float height)
{
    m_ribbonHeight = height;
}


// ----------------------------------------------------------------------------
// Communication to the simulation
// ----------------------------------------------------------------------------

void RibbonProbe::notifySimulation()
{
    std::vector<osg::Vec3> array;
    osg::Matrixd M;

    m_dragger->computeLocalToWorldMatrix(M, NULL);

    for (unsigned int i = 0; i < m_numRibbons; i++) {
        float offset = (float)i * (m_emitterHeight / (float)(m_numRibbons - 1));

	array.push_back(osg::Vec3(0.0f, 0.0f, offset - m_emitterHeight/2.0) * M);
    }

    double absoluteRibbonHeight = m_emitterHeight * (m_ribbonHeight/100.0f) / m_numRibbons;

    vertex_t *vec = new vertex_t[array.size()];
    for(unsigned int i = 0; i < array.size(); i++)
    {
        vec2vertex(array[i] , &vec[i]);
    }

    if(modelConn != NULL) {
        modelConn->startBinaryPush("sim::newSample");

        char *templ;
        asprintf(&templ, "int;int;int;double;int;{double;double;double}[%d]", array.size());
        modelConn->binaryPush(templ, id(), m_sampleType, PROBETYPE_RIBBON, absoluteRibbonHeight, array.size(), vec);
        modelConn->stopBinaryPush();
        free(templ);
    }else  {
        FAN_postMessage(masterCom, "login", NULL, NULL);
    }


    delete[] vec;
}


// ----------------------------------------------------------------------------
// Communication from the simulation
// ----------------------------------------------------------------------------

void RibbonProbe::setData(ribbon_probe_data *data)
{
    m_updateLock.lock();

    if (m_lastSample) {
        zap_array(m_lastSample->num_vertices);
        zap_array(m_lastSample->ribbons);
        zap_array(m_lastSample->values);
        zap(m_lastSample);
    }

    m_lastSample = data;

    m_updateLock.unlock();
}

void RibbonProbe::update()
{
    // skip the update if there is no new sample or we're dragging
    if (!m_lastSample || !m_colorRange.size()) /* || m_dragger->isDragging()*/
        return;

    m_updateLock.lock();

    osg::Matrixd M;

    m_dragger->computeLocalToWorldMatrix(M, NULL);

    M = osg::Matrixd::inverse(M);

    // remove all drawables from the ribbon geode
    m_ribbonGeode->removeDrawable(0, m_ribbonGeode->getNumDrawables());

    m_lastMinimum = m_lastSample->min_value;
    m_lastMaximum = m_lastSample->max_value;

    // recreate the scalarbar if min/max changed
    const osgSim::ScalarsToColors* currentSTC = m_scalarBar->getScalarsToColors();
    
    if (!m_customMinMax) {
        m_colorRangeMin = m_lastSample->min_value;
        m_colorRangeMax = m_lastSample->max_value;
    }

    if (currentSTC->getMin() != m_colorRangeMin || currentSTC->getMax() != m_colorRangeMax) {
        m_scalarBar->setScalarsToColors(new osgSim::ColorRange(m_colorRangeMin, m_colorRangeMax, m_colorRange));
    }
    currentSTC = m_scalarBar->getScalarsToColors();

    int index = 0;

    // create one triangle strip per ribbon
    for (unsigned int i = 0; i < m_lastSample->num_ribbons; i++) {
    	// vertices per ribbon
    	unsigned int numVertices = m_lastSample->num_vertices[i];

        if (numVertices == 0)
          continue;

        osg::Vec3Array* vertices = new osg::Vec3Array(numVertices);
        osg::Vec4Array* colors = new osg::Vec4Array(numVertices);

        for (unsigned int j = 0; j < numVertices; j++) {
            // vertices
            (*vertices)[j].set(m_lastSample->ribbons[index].x,
                               m_lastSample->ribbons[index].y,
                               m_lastSample->ribbons[index].z);

	    (*vertices)[j] = (*vertices)[j]*M;

            // map values to colors
            (*colors)[j] = currentSTC->getColor(m_lastSample->values[index/2]);

	    index++;
        }

        // create the geometry to hold the data
        osg::Geometry* ribbon = new osg::Geometry();
        ribbon->setVertexArray(vertices);
        ribbon->setColorArray(colors);
        ribbon->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, numVertices));
        ribbon->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        // disable lighting
        osg::StateSet* ss = ribbon->getOrCreateStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        m_ribbonGeode->addDrawable(ribbon);
    }

    zap_array(m_lastSample->num_vertices);
    zap_array(m_lastSample->ribbons);
    zap_array(m_lastSample->values);
    zap(m_lastSample);

    m_updateLock.unlock();
}
