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

#ifndef PROBE_H
#define PROBE_H

#include <osg/Group>
#include <osg/Geode>
#include <osg/Switch>
#include <osgSim/ColorRange>
#include <osgSim/ScalarBar>
#include <OpenThreads/Mutex>

#include "simRemoteTypes.h"
#include "TransformDragger.h"
#include "TranslationDragger.h"
#include "RotationDragger.h"
#include "SelectableMixin.h"


class Probe : public osg::Group, public SelectableMixin, public DraggerDelegate
{
protected:
    OpenThreads::Mutex m_updateLock;

    osg::ref_ptr<osg::Geode> m_emitterGeometry;
    osg::ref_ptr<TransformDragger> m_dragger;
    osg::ref_ptr<osg::Switch> m_selectionSwitch;
    osg::ref_ptr<osgSim::ScalarBar> m_scalarBar;

    std::vector<osg::Vec4> m_colorRange;

    unsigned int m_id;
    unsigned int m_colormap;
    bool m_customMinMax;
    double m_colorRangeMin;
    double m_colorRangeMax;

    double m_lastMinimum;
    double m_lastMaximum;

    sample_type m_sampleType;

    /// Create the OSG nodes for the emitter
    virtual void createEmitterGeometry() {}

    virtual void handleSelected();
    virtual void handleDeselected();

    /// Assign the numeric id to the node name
    void assignIDToName();

    virtual void draggerDidMove();
    virtual void draggerDidEndDragging(bool hasChanged);

    void printObjectInfo();

    Probe() {}

public:
    /// Constructor
    Probe(unsigned int id);
    /// Destructor
    virtual ~Probe();

    /// Notify the simulation of a change
    virtual void notifySimulation() = 0;

    /// Return the ID of this probe
    unsigned int id() const { return m_id; }

    /// Set the position
    void setPosition(const osg::Vec3& pos);
    /// Get the position
    osg::Vec3 position() const;
    /// Set the attitude (rotation)
    void setAttitude(const osg::Quat& quat);
    /// Get the attitude (rotation)
    osg::Quat attitude() const;

    /// Set the colormap to use
    void setColorRange(const std::vector<osg::Vec4>& colors, float min, float max);
    /// Set the colormap to use by Index
    void setColorRangeIndex(unsigned int index, float min = 0.0f, float max = 1.0f);
    /// Get the colormap index
    unsigned int colorRangeIndex() { return m_colormap; }
    /// Set the usage of custom Min/Max values
    void setCustomMinMax(bool custom) { m_customMinMax = custom; }
    /// Get the usage of custom Min/Max values
    bool customMinMax() { return m_customMinMax; }
    /// Set the minimum of the color range
    void setColorRangeMin(double min) { m_colorRangeMin = min; }
    /// Get the minimum of the color range
    double colorRangeMin() { return m_colorRangeMin; }
    /// Set the maximum of the color range
    void setColorRangeMax(double max) { m_colorRangeMax = max; }
    /// Get the maximum of the color range
    double colorRangeMax() { return m_colorRangeMax; }

    /// Get minimum of the current data set
    double dataMinimum() { return m_lastMinimum; }
    /// Get maximum of the current data set
    double dataMaximum() { return m_lastMaximum; }

    /// Set what to sample
    void setSampleType(sample_type type);
    /// Get what this probe is sampling
    sample_type sampleType() const { return m_sampleType; }

    /// Get the geometry of the emitter
    osg::ref_ptr<osg::Geode> emitterGeometry() const { return m_emitterGeometry; }
    
    /// Get the type of the probe
    virtual probe_type probeType() = 0;

    /// Update the data
    virtual void update() = 0;
};

#endif // PROBE_H
