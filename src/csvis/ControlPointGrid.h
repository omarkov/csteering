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

#ifndef CONTROLPOINTGRID_H
#define CONTROLPOINTGRID_H

#include <vector>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include "DraggerDelegate.h"
#include "ControlPoint.h"


/**
 * A ControlPointGrid consists of U * V control points as dictated
 * by the modeling component and is part of a face.
 */
class ControlPointGrid : public osg::Group, public DraggerDelegate
{
    // hold a pointer to the points
    std::vector<ControlPoint*> m_controlPoints;

    unsigned int m_faceID;
    unsigned int m_uSize;
    unsigned int m_vSize;

    ControlPoint* m_currentSelection;

    bool m_isShown;

    /// private default constructor
    ControlPointGrid() {}

    /// Create the OSG nodes
    void createNodes(const face_t& face);

protected:
    virtual void draggerDidMove();
    /// A control point finished moving 
    virtual void draggerDidEndDragging(bool hasChanged);

public:
    META_Node(csvis,ControlPointGrid);
    
    /// Constructor
    ControlPointGrid(const face_t& face);
    /// Copy Constructor
    ControlPointGrid(const ControlPointGrid&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    /// Return the width of the grid
    unsigned int uSize() const { return m_uSize; }
    /// Return the height of the grid
    unsigned int vSize() const { return m_vSize; }

    /// Set the shown status for optimization
    void setShown(bool shown) { m_isShown = shown; }
    /// Is this grid shown?
    bool isShown() const { return m_isShown; }

    /// Select a specific control point
    void selectControlPoint(unsigned int id);
    /// Deselect all currently selected points
    void deselectAll();

    // Rescale all control points
    void rescale();
};

#endif // CONTROLPOINTGRID_H
