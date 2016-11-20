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

#include "GlyphProbe.h"
#include "SliceProbe.h"
#include "RibbonProbe.h"
#include "RibbonProbeWindow.h"
#include "GlyphProbeWindow.h"
#include "SliceProbeWindow.h"
#include "Application.h"
#include "common.h"
#include "ProbeManager.h"
#include "FANClasses.h"
#include "CommonServer.h"
#include "GUIController.h"


ProbeManager* g_ProbeManager = 0;


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

ProbeManager::ProbeManager() : m_currentID(1), m_currentSelection(0)
{
}

bool ProbeManager::init(osg::Group* rootNode)
{
    m_probeGroup = new osg::Group;
    rootNode->addChild(m_probeGroup.get());

    return true;
}


// ----------------------------------------------------------------------------
// Probe management
// ----------------------------------------------------------------------------

void ProbeManager::addProbe(probe_type type)
{
    Probe* probe = NULL;
    unsigned int id = m_currentID++; // we always generate a new id

    // create a new probe
    switch (type) {
        case PROBETYPE_GLYPH:
            probe = new GlyphProbe(id);
            break;

        case PROBETYPE_RIBBON:
            probe = new RibbonProbe(id);
            break;

        case PROBETYPE_PLANE:
            probe = new SliceProbe(id);
            break;

        default:
            return;
    }

    m_probeMap[probe->id()] = probe;
    m_probeGroup->addChild(probe);

    /*
    // orient the probe (somehow)
    {
        osg::Matrixd viewMatrix = g_Application->viewer()->getViewMatrix();
        osg::Vec3 eye, center, up;

        viewMatrix.getLookAt(eye, center, up);

        center.normalize();
        probe->setPosition(eye - center * 200.0f);

        osg::Quat quat;
        quat.makeRotate(osg::Vec3(0.0f, -1.0f, 0.0f), eye - center);
        probe->setAttitude(quat);
    }
    */

    probe->setColorRangeIndex(2);
    probe->setCustomMinMax(false);
    probe->setSampleType(SAMPLETYPE_PRESSURE);

    switch (type) {
        case PROBETYPE_GLYPH:
            g_GlyphProbeWindow->showWindow((GlyphProbe*)probe);
            break;

        case PROBETYPE_RIBBON:
            g_RibbonProbeWindow->showWindow((RibbonProbe*)probe);
            break;

        case PROBETYPE_PLANE:
            g_SliceProbeWindow->showWindow((SliceProbe*)probe);
            break;
            
        default:
            break;
    }
}

void ProbeManager::removeProbe(unsigned int id)
{
    deselectProbe();

    Probe* probe = findProbe(id);

    if (!probe) {
      //error("Tried to remove non-existent probe with id %d.", id);
        return;
    }

    // remove the probe from the simulation
    if(modelConn != NULL)
    {
        char *cid;
        asprintf(&cid, "%d", id);
        modelConn->rpc("sim::deleteSample", 1, cid);
        free(cid);
    }else
    {
        FAN_postMessage(masterCom, "login", NULL, NULL);
    }


    // remove the probe from the scenegraph
    m_probeGroup->removeChild(probe);

    m_probeMap.erase(m_probeMap.find(id));
}

void ProbeManager::removeAllProbes()
{
    deselectProbe();

    std::map<unsigned int, Probe*>::iterator it;

    for (it = m_probeMap.begin(); it != m_probeMap.end(); ++it)
        m_probeGroup->removeChild(it->second);

    m_probeMap.clear();
}

Probe* ProbeManager::findProbe(unsigned int id)
{
    if (m_probeMap.find(id) != m_probeMap.end())
        return m_probeMap[id];
    else
        return NULL;
}


// ----------------------------------------------------------------------------
// Probe selection
// ----------------------------------------------------------------------------

void ProbeManager::selectProbe(unsigned int id)
{
    Probe* probe = findProbe(id);

    if (!probe) {
        error("Tried to select non-existent probe with id %d.", id);
        return;
    }

    deselectProbe();
    probe->setSelectionState(SELECTED);
    m_currentSelection = probe;
    
    g_GUIController->enableMenuItems("DESELECT_PROBE");
    g_GUIController->enableMenuItems("EDIT_PROBE");
    g_GUIController->enableMenuItems("CONFIRM_REMOVE_PROBE");
    g_GUIController->enableMenuItems("REMOVE_PROBE");
    g_GUIController->enableMenuItems("SNAP_PROBE_ROTATION_45");
    g_GUIController->enableMenuItems("SNAP_PROBE_ROTATION_90");
}

void ProbeManager::deselectProbe()
{
    if (!m_currentSelection)
        return;

    m_currentSelection->setSelectionState(DESELECTED);
    m_currentSelection = NULL;
    
    g_GUIController->disableMenuItems("DESELECT_PROBE");
    g_GUIController->disableMenuItems("EDIT_PROBE");
    g_GUIController->disableMenuItems("CONFIRM_REMOVE_PROBE");
    g_GUIController->disableMenuItems("REMOVE_PROBE");
    g_GUIController->disableMenuItems("SNAP_PROBE_ROTATION_45");
    g_GUIController->disableMenuItems("SNAP_PROBE_ROTATION_90");
}


// ----------------------------------------------------------------------------
// Data update
// ----------------------------------------------------------------------------

void ProbeManager::updateSliceProbe(planar_sample *data)
{
    // check parameter
    if (!data) {
        error("Received empty data packet for slice probe.");
        return;
    }

    // find the probe
    SliceProbe* probe = dynamic_cast<SliceProbe*>(findProbe(data->id));
    if (!probe)
        return;

    probe->setData(data);
}

void ProbeManager::updateRibbonProbe(ribbon_probe_data *data)
{
    // check parameter
    if (!data) {
        error("Received empty data packet for ribbon probe.");
        return;
    }

    // find the probe
    RibbonProbe* probe = dynamic_cast<RibbonProbe*>(findProbe(data->id));
    if (!probe)
        return;

    probe->setData(data);
}

void ProbeManager::updateGlyphProbe(glyph_probe_data *data)
{
    // check parameter
    if (!data) {
        error("Received empty data packet for a glyph probe.");
        return;
    }

    // find the probe
    GlyphProbe* probe = dynamic_cast<GlyphProbe*>(findProbe(data->id));
    if (!probe)
        return;

    probe->setData(data);
}
