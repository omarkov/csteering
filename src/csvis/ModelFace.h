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

#ifndef MODELFACE_H
#define MODELFACE_H

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Switch>

#include "simRemoteTypes.h"
#include "ControlPointGrid.h"
#include "SelectableMixin.h"


class ModelFace : public osg::Group, public SelectableMixin
{
    osg::ref_ptr<osg::Geode> m_faceGeode;
    osg::ref_ptr<osg::Switch> m_selectionSwitch;
    osg::ref_ptr<ControlPointGrid> m_controlPoints;

    unsigned int m_faceID;

    // create the necessary nodes for the main face
    void createNodes(const face_t& face);

    // calculate and return surface normals
    osg::Vec3Array* calculateSurfaceNormals(osg::Geometry* geometry);
    // calculate and set vertex normals
    void calculateVertexNormals(osg::Geometry* geometry);

    // private constructor
    ModelFace() {}

protected:
    void handleSelected();
    void handleDeselected();

public:
    META_Node(csvis,ModelFace);

    // Constructor
    ModelFace(const face_t& face);
    // Copy Constructor
    ModelFace(const ModelFace&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    unsigned int id() const { return m_faceID; }

    osg::ref_ptr<osg::Geode> geode() const { return m_faceGeode; }

    void selectControlPoint(unsigned int index);
    void rescaleControlPoints();
};

#endif // MODELFACE_H
