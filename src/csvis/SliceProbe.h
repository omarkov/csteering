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

#ifndef SLICE_PROBE_H
#define SLICE_PROBE_H

#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>

#include "Probe.h"
#include "simRemoteTypes.h"


class SliceProbe : public Probe
{
    osg::Texture2D* m_texture;

    float m_sliceWidth;
    float m_sliceHeight;

    planar_sample *m_lastSample;

    void updateEmitterGeometry();

    osg::ref_ptr<osg::PositionAttitudeTransform> m_topRight;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_topLeft;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_bottomLeft;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_bottomRight;

    /// Private default constructor
    SliceProbe() {}

protected:
    virtual void createEmitterGeometry();

public:
    META_Node(csvis, SliceProbe);

    /// Constructor
    SliceProbe(unsigned int id);
    /// Copy Constructor
    SliceProbe(const SliceProbe&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);
    /// Destructor
    virtual ~SliceProbe();

    /// Set a new width for this probe
    void setWidth(float width);
    /// Get the current width of this probe
    float width() const;
    /// Set a new height for this probe
    void setHeight(float height);
    /// Get the current height of this probe
    float height() const;

    /// Set new data
    void setData(planar_sample *sample);
    /// Update data (only called by the update callback)
    virtual void update();

    virtual void notifySimulation();
    
    /// Get the type of the probe
    virtual probe_type probeType() { return PROBETYPE_PLANE; }

    void getPoints(osg::Vec3& top_left, osg::Vec3& top_right, osg::Vec3& bottom_left, osg::Vec3& bottom_right);
};

#endif // SLICE_PROBE_H
