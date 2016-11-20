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

#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <osg/Group>
#include <osg/Geode>
#include <osg/Switch>

#include "simRemoteTypes.h"
#include "TranslationDragger.h"
#include "SelectableMixin.h"

/**
 * A ControlPoint is used for modifying a face in the model and is organised
 * into a grid (ControlPointGrid). It can be selected and dragged around with
 * the pointer device.
 */
class ControlPoint : public osg::Group, public SelectableMixin
{
    unsigned int m_id;

    osg::ref_ptr<TranslationDragger> m_dragger;
    osg::ref_ptr<osg::Switch> m_selectionSwitch;
    osg::ref_ptr<osg::Geode> m_selectedGeode;
    osg::ref_ptr<osg::Geode> m_deselectedGeode;

    /// Create the OSG nodes
    void createNodes();
    /// Create the geometry
    void createGeometry();

    /// Private default constructor
    ControlPoint() {}

protected:
    /// Control point was selected
    void handleSelected();
    /// Control point was deselected
    void handleDeselected();

public:
    META_Node(csvis,ControlPoint);

    /// Constructor
    ControlPoint(unsigned int id);
    /// Copy Constructor
    ControlPoint(const ControlPoint&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    /// Return the OCC ID of this point
    unsigned int id() const { return m_id; }

    /// Set the position of this control point
    void setPosition(const osg::Vec3& pos);
    /// Get the current position
    osg::Vec3 position() const;

    /// Set the dragging delegate
    void setDraggerDelegate(DraggerDelegate*);

    /// Print info about this point into the object info pane
    void printObjectInfo();

    /// Rescale the geometry of this point
    void rescale();
};


#endif // CONTROLPOINT_H
