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

#ifndef GLYPH_PROBE_H
#define GLYPH_PROBE_H

#include <osg/Geode>

#include "Probe.h"


class GlyphProbe : public Probe
{
    osg::ref_ptr<osg::Geode> m_glyphGeode;

    float m_emitterHeight;
    unsigned int m_numStreams;

    glyph_probe_data *m_lastSample;

    /// Private default constructor
    GlyphProbe() {}

protected:
    void createEmitterGeometry();

public:
    META_Node(csvis,GlyphProbe);

    /// Constructor
    GlyphProbe(unsigned int id);
    /// Copy Constructor
    GlyphProbe(const GlyphProbe&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);
    /// Destructor
    virtual ~GlyphProbe();

    /// Set the height of the emitter
    void setEmitterHeight(float height);
    /// Get the height of the emitter
    float emitterHeight() const { return m_emitterHeight; }
    /// Set the number of streams
    void setNumStreams(unsigned int num);
    /// Get the number of streams
    unsigned int numStreams() const { return m_numStreams; }

    /// Update the visualized data
    void setData(glyph_probe_data *data);
    virtual void update();

    virtual void notifySimulation();
    
    /// Get the type of the probe
    virtual probe_type probeType() { return PROBETYPE_GLYPH; }
};

#endif // GLYPH_PROBE_H
