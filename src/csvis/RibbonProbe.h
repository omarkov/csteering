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

#ifndef RIBBON_PROBE_H
#define RIBBON_PROBE_H

#include <osg/Geode>

#include "Probe.h"
#include "simRemoteTypes.h"


class RibbonProbe : public Probe
{
    osg::ref_ptr<osg::Geode> m_ribbonGeode;

    void createRibbonGeometry();

    unsigned int m_numRibbons;
    float m_ribbonHeight;
    float m_emitterHeight;

    ribbon_probe_data *m_lastSample;

    /// Private default constructor
    RibbonProbe() {}

protected:
    void createEmitterGeometry();

public:
    META_Node(csvis, RibbonProbe);

    /// Constructor
    RibbonProbe(unsigned int id);
    /// Copy Constructor
    RibbonProbe(const RibbonProbe&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);
    /// Destructor
    virtual ~RibbonProbe();

    /// Set the number of ribbons
    void setNumRibbons(unsigned int num);
    /// Get the number of ribbons
    unsigned int numRibbons() const { return m_numRibbons; }
    /// Set the height of individual ribbons
    void setRibbonHeight(float height);
    /// Get the height of the ribbons
    float ribbonHeight() const { return m_ribbonHeight; }
    /// Set the height of the emitter
    void setEmitterHeight(float height);
    /// Get the height of the emitter
    float emitterHeight() const { return m_emitterHeight; }

    /// Update the visualized data
    void setData(ribbon_probe_data *data);
    virtual void update();

    virtual void notifySimulation();
    
    /// Get the type of the probe
    virtual probe_type probeType() { return PROBETYPE_RIBBON; }
};

#endif // RIBBON_PROBE_H
