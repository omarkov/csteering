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

#include "common.h"
#include "Application.h"
#include "FANClasses.h"
#include "CommonServer.h"
#include "NodeFlags.h"
#include "SliceProbe.h"

// hax, hax
#ifdef __DARWIN_OSX__
#define MINFLOAT FLT_MIN
#else
#include <values.h>
#endif


// ----------------------------------------------------------------------------
// Creation
// ----------------------------------------------------------------------------

SliceProbe::SliceProbe(unsigned int id) : Probe(id)
{
    m_texture = NULL;
    m_lastSample = NULL;
    m_sliceWidth = 1.0f;
    m_sliceHeight = 1.0f;

    createEmitterGeometry();
}

SliceProbe::SliceProbe(const SliceProbe& other, const osg::CopyOp& op) : Probe(other.m_id)
{
    m_texture = other.m_texture;
    m_lastSample = NULL;
    m_sliceWidth = other.m_sliceWidth;
    m_sliceHeight = other.m_sliceHeight;
    m_lastMinimum = other.m_lastMinimum;
    m_lastMaximum = other.m_lastMaximum;

    createEmitterGeometry();
}

SliceProbe::~SliceProbe()
{
    if (m_lastSample) {
        zap_array(m_lastSample->values);
        zap(m_lastSample);
    }
}


void SliceProbe::createEmitterGeometry()
{
    double f = g_Application->model()->scalingFactor();
    m_sliceWidth = 0.25f * f;
    m_sliceHeight = 0.25f * f;

    float w = m_sliceWidth / 2.0f;
    float h = m_sliceHeight / 2.0f;

    // vertices
    osg::Vec3Array* vertices = new osg::Vec3Array(4);
    (*vertices)[0].set(-w, -h, 0.0f); // top left
    (*vertices)[1].set(-w, h, 0.0f); // bottom left
    (*vertices)[2].set(w, h, 0.0f); // bottom right
    (*vertices)[3].set(w, -h, 0.0f); // top right

    // color
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

    // texture coordinates
    osg::Vec2Array* texcoords = new osg::Vec2Array(4);
    (*texcoords)[0].set(0.0f,1.0f);
    (*texcoords)[1].set(0.0f,0.0f);
    (*texcoords)[2].set(1.0f,0.0f);
    (*texcoords)[3].set(1.0f,1.0f);

    // create a new texture
    osg::Texture2D* texture = new osg::Texture2D;
    texture->setDataVariance(osg::Object::DYNAMIC);
    m_texture = texture;

    osg::Geometry* drawable = new osg::Geometry;
    drawable->setVertexArray(vertices);
    drawable->setColorArray(colors);
    drawable->setColorBinding(osg::Geometry::BIND_OVERALL);
    drawable->setTexCoordArray(0, texcoords);
    drawable->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    // add the texture to the drawable and disable lighting
    osg::StateSet* ss = drawable->getOrCreateStateSet();
    ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    m_emitterGeometry->addDrawable(drawable);
    m_dragger->addChild(m_emitterGeometry.get());

    assignIDToName();
}


// ----------------------------------------------------------------------------
// Change of the emitter geometry
// ----------------------------------------------------------------------------

/**
 * Recreates the slice polygon upon a change of parameters.
 */
void SliceProbe::updateEmitterGeometry()
{
    float w = m_sliceWidth / 2.0f;
    float h = m_sliceHeight / 2.0f;

    osg::Vec3Array* vertices = new osg::Vec3Array(4);
    (*vertices)[0].set(-w, -h, 0.0f); // top left
    (*vertices)[1].set(-w, h, 0.0f); // bottom left
    (*vertices)[2].set(w, h, 0.0f); // bottom right
    (*vertices)[3].set(w, -h, 0.0f); // top right

    osg::Geometry* drawable = dynamic_cast<osg::Geometry*>(m_emitterGeometry->getDrawable(0));
    drawable->setVertexArray(vertices);
}

/**
 * Set a new width for the probe. Will recreate geometry.
 *
 * @param width the new width
 */
void SliceProbe::setWidth(float width)
{
    m_sliceWidth = width;
    updateEmitterGeometry();
}

/**
 * Returns the current width of the probe.
 *
 * @return the current width
 */
float SliceProbe::width() const
{
    return m_sliceWidth;
}

/**
 * Set a new height for the probe. Will recreate geometry
 *
 * @param height the new height
 */
void SliceProbe::setHeight(float height)
{
    m_sliceHeight = height;
    updateEmitterGeometry();
}

/**
 * Returns the current height of the probe
 *
 * @return the current height
 */
float SliceProbe::height() const
{
    return m_sliceHeight;
}

// ----------------------------------------------------------------------------
// Communication to the simulation
// ----------------------------------------------------------------------------

/**
 * Notifies the simulation of this probe upon creation.
 */
void SliceProbe::notifySimulation()
{
    float w = m_sliceWidth / 2.0f;
    float h = m_sliceHeight / 2.0f;

    // calculate 4 corners
    // FIXME: this assumes that the dragger is under no other transformation node
    osg::Matrixd M;
    m_dragger->computeLocalToWorldMatrix(M, NULL);

    vertex_t *vec = new vertex_t[4];
    vec2vertex(osg::Vec3f(-w, h, 0.0f) * M , &vec[0]); // bottom left
    vec2vertex(osg::Vec3f(w, h, 0.0f) * M, &vec[1]); // bottom right
    vec2vertex(osg::Vec3f(w, -h, 0.0f) * M, &vec[2]); // top right
    vec2vertex(osg::Vec3f(-w, -h, 0.0f) * M , &vec[3]); // top left

    if(modelConn != NULL) {	
        modelConn->startBinaryPush("sim::newSample");
        modelConn->binaryPush("int;int;int;{double;double;double}[4]", id(), m_sampleType, PROBETYPE_PLANE, vec);
        modelConn->stopBinaryPush();
    }else
    {
        FAN_postMessage(masterCom, "login", NULL, NULL);
    }

    delete[] vec;
}


// ----------------------------------------------------------------------------
// Communication from the simulation
// ----------------------------------------------------------------------------

/**
 * Updates the visualized data.
 *
 * @param sample a planar sample from the simulation
 */
void SliceProbe::setData(planar_sample *sample)
{
    m_updateLock.lock();

    // kill the last sample
    if (m_lastSample) {
        zap_array(m_lastSample->values);
        zap(m_lastSample);
    }

    // allocate a new sample
    m_lastSample = sample;

    m_updateLock.unlock();
}


void SliceProbe::update()
{
    // do we have anything to update?
    if (!m_lastSample || !m_colorRange.size())
        return;

    // we only handle scalar data for now
    if (m_lastSample->type != DATATYPE_SCALAR || m_lastSample->dimensionality != 1) {
        error("Got planar sample with non-scalar data. This is currently unsupported by slice probes.");
        return;
    }
    
    m_lastMinimum = m_lastSample->min;
    m_lastMaximum = m_lastSample->max;

    m_updateLock.lock();

    // recreate the scalarbar if min/max changed
    const osgSim::ScalarsToColors* currentSTC = m_scalarBar->getScalarsToColors();
    if (!m_customMinMax) {
        m_colorRangeMin = m_lastSample->min;
        m_colorRangeMax = m_lastSample->max;
    }
    if (currentSTC->getMin() != m_colorRangeMin || currentSTC->getMax() != m_colorRangeMax) {
        m_scalarBar->setScalarsToColors(new osgSim::ColorRange(m_colorRangeMin, m_colorRangeMax, m_colorRange));
    }


    osg::Image* image = new osg::Image();
    image->setImage(m_lastSample->u_size,
                    m_lastSample->v_size,
                    1,
                    3,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    new unsigned char[m_lastSample->u_size * m_lastSample->v_size * 3],
                    osg::Image::USE_NEW_DELETE);

    unsigned char* ptr = image->data();

    currentSTC = m_scalarBar->getScalarsToColors();

    for (unsigned int i = 0; i < m_lastSample->u_size * m_lastSample->v_size; i++) {
        float value = m_lastSample->values[i];

	if(value > MINFLOAT)
	{
        	// clamp
        	if (value >= m_colorRangeMax) value = m_colorRangeMax;
        	if (value <= m_colorRangeMin) value = m_colorRangeMin;
	}

	osg::Vec4 color;

	if(value == MINFLOAT)
	{
        	color = osg::Vec4(0.0, 0.0, 0.0, 0.0);
	}else
	{
        	color = currentSTC->getColor(value);
	}

        *ptr++ = (unsigned char)(color.x() * 255);
        *ptr++ = (unsigned char)(color.y() * 255);
        *ptr++ = (unsigned char)(color.z() * 255);
    }
 
    m_texture->setImage(image);

    zap_array(m_lastSample->values);
    zap(m_lastSample);

    m_updateLock.unlock();
}
