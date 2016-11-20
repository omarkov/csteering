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

#ifndef TRIANGULATEDMODEL_H
#define TRIANGULATEDMODEL_H

#include <map>

#include <OpenThreads/Mutex>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Geometry>

#include "ModelFace.h"


class TriangulatedModel : public osg::Group
{
    OpenThreads::Mutex m_updateLock;
    std::vector<face_t*> m_pendingFaces;

    /// Transform of the model
    osg::ref_ptr<osg::PositionAttitudeTransform> m_transform;
    /// Bounding box of the model
    osg::BoundingBox m_boundingBox;

    /// A map id <-> face object
    std::map<unsigned int, osg::ref_ptr<ModelFace> > m_faceMap;

    /// Pointer to the current selection (or NULL if deselected)
    ModelFace* m_currentSelection;

    void addModelFace(ModelFace* face);

public:
    META_Node(csvis,TriangulatedModel)

    /// Default Constructor
    TriangulatedModel();
    /// Copy Constructor
    TriangulatedModel(const TriangulatedModel&,
		      const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

    /// Update data of a face
    void updateFace(const face_t& face);
    /// Select a face
    void selectFace(unsigned int id);
    /// Deselect the currently selected face
    void deselectFace();
    /// Return the currently selected face
    ModelFace* selectedFace() { return m_currentSelection; }

    /// Get the asymmetric scaling factors of the BB
    void scalingFactors(double& fx, double& fy, double& fz);
    /// Get the symmetric scaling factor of the BS
    double scalingFactor();
    /// Rescale all necessary geometry
    void rescale();

    /// Set the attitude of the model
    void setAttitude(const osg::Quat& quat);
    /// Get the attitude of the model
    const osg::Quat& attitude() const;

    void processPendingFaces();
    void enableCallbacks();

};

#endif // TRIANGULATEDMODEL_H
