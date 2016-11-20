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

#include <osg/GL>
#include <iostream>
#include "FANClasses.h"
#include "CommonServer.h"
#include "ControlPoint.h"
#include "ControlPointGrid.h"


// ----------------------------------------------------------------------------
// Callback responsible for drawing the gridlines
// ----------------------------------------------------------------------------
/*
void ControlPointGrid::GridlinesCallback::drawImplementation(osg::State& state, const osg::Drawable* drawable) const
{
    unsigned int u, v;
    unsigned int u_size = m_grid->uSize();
    unsigned int v_size = m_grid->vSize();

    // FIXME: this clobbers the state
    glDisable(GL_LIGHTING);
    glColor4f(0.8f, 0.0f, 0.0f, 0.8f);

    // draw horizontal (u) lines
    for (v = 0; v < v_size; v++) {
	glBegin(GL_LINES);
	for (u = 0; u < u_size; u++) {
	    osg::Vec3 pos = m_grid->m_positions[u + v * u_size];
	    glVertex3f(pos.x(), pos.y(), pos.z());
	}
	glEnd();
    }

    // draw vertical (v) lines
    for (u = 0; u < u_size; u++) {
	glBegin(GL_LINES);
	for (v = 0; v < v_size; v++) {
	    osg::Vec3 pos = m_grid->m_positions[u + v * u_size];
	    glVertex3f(pos.x(), pos.y(), pos.z());
	}
	glEnd();
    }

    glEnable(GL_LIGHTING);

    drawable->drawImplementation(state);
}
*/


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

ControlPointGrid::ControlPointGrid(const face_t& face)
{
    m_uSize = face.u_size;
    m_vSize = face.v_size;
    m_faceID = face.id;

    m_currentSelection = NULL;

    m_isShown = false;

    createNodes(face);
}

// FIXME
ControlPointGrid::ControlPointGrid(const ControlPointGrid& other, const osg::CopyOp& op)
    : osg::Group(other, op)
{
    m_uSize = other.m_uSize;
    m_vSize = other.m_vSize;
    m_faceID = other.m_faceID;
    m_isShown = other.m_isShown;
}



void ControlPointGrid::rescale()
{
    std::vector<ControlPoint*>::const_iterator it;

    for (it = m_controlPoints.begin(); it != m_controlPoints.end(); ++it)
        (*it)->rescale();
}


// ----------------------------------------------------------------------------
// Node creation
// ----------------------------------------------------------------------------

void ControlPointGrid::createNodes(const face_t& face)
{
    // create the control points
    for (unsigned int v = 0; v < face.v_size; v++) {
        for (unsigned int u = 0; u < face.u_size; u++) {
            osg::Vec3 position;

            // get the position of the control point
            unsigned int index = u + v * face.u_size;
            position.set((float)face.control_points[index].x,
                         (float)face.control_points[index].y,
                         (float)face.control_points[index].z);

            ControlPoint *cp = new ControlPoint(index);
            cp->setPosition(position);
            cp->setDraggerDelegate(this);

            m_controlPoints.push_back(cp);
            addChild(cp);
	}
    }
}


// ----------------------------------------------------------------------------
// Selection
// ----------------------------------------------------------------------------

void ControlPointGrid::selectControlPoint(unsigned int id)
{
    if (id < m_controlPoints.size()) {
        if (m_currentSelection)
            m_currentSelection->setSelectionState(DESELECTED);

        m_controlPoints[id]->setSelectionState(SELECTED);

        m_currentSelection = m_controlPoints[id];
    }
}

void ControlPointGrid::deselectAll()
{
    if (m_currentSelection) {
        m_currentSelection->setSelectionState(DESELECTED);
        m_currentSelection = NULL;
    }
}


// ----------------------------------------------------------------------------
// Dragger delegate
// ----------------------------------------------------------------------------

void ControlPointGrid::draggerDidMove()
{
    m_currentSelection->printObjectInfo();
};

void ControlPointGrid::draggerDidEndDragging(bool hasChanged)
{
    if (!hasChanged)
        return;

    if(modelConn != NULL)
    {
        vertex_t *array = new vertex_t[m_controlPoints.size()];

        for (unsigned int i = 0; i < m_controlPoints.size(); i++) {
            array[i].x = m_controlPoints[i]->position().x();
            array[i].y = m_controlPoints[i]->position().y();
            array[i].z = m_controlPoints[i]->position().z();
        }

        char *templ;
        asprintf(&templ, "int;{double;double;double}[%d]", m_controlPoints.size());
        modelConn->startBinaryPush("model::changePoles");
        modelConn->binaryPush(templ,m_faceID,array);
        modelConn->stopBinaryPush();
        free(templ);

        delete[] array;
    }else
    {
         FAN_postMessage(masterCom, "login", NULL, NULL);
    }
}
