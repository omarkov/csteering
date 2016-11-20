#// -*- Mode: C++; indent-tabs-mode: nil -*-
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

#ifndef PROBEMANAGER_H
#define PROBEMANAGER_H

#include <osg/Group>

#include "Probe.h"
#include "simRemoteTypes.h"


class ProbeManager
{
    std::map<unsigned int, Probe*> m_probeMap;
    osg::ref_ptr<osg::Group> m_probeGroup;

    unsigned int m_currentID;

    Probe* m_currentSelection;

    Probe* findProbe(unsigned int id);

public:
    ProbeManager();

    bool init(osg::Group* rootNode);

    void addProbe(probe_type type);
    void removeProbe(unsigned int id);
    void removeAllProbes();

    void selectProbe(unsigned int id);
    Probe* selectedProbe() { return m_currentSelection; }
    void deselectProbe();

    void updateSliceProbe(planar_sample *data);
    void updateRibbonProbe(ribbon_probe_data *data);
    void updateGlyphProbe(glyph_probe_data *data);
};


extern ProbeManager* g_ProbeManager;

#endif // PROBEMANAGER_H
