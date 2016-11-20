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

#include <assert.h>
#include <fstream>
#include <FANClasses.h>

#include <osg/ShapeDrawable>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/TriStripVisitor>

#include "Application.h"
#include "TriangulatedModel.h"



// ----------------------------------------------------------------------------
// Face Update calllback
// ----------------------------------------------------------------------------

class ModelFaceUpdateCallback : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
    {
        TriangulatedModel *model = dynamic_cast<TriangulatedModel*>(node);

        if (model) model->processPendingFaces();

        traverse(node, nv);
    }
};

void TriangulatedModel::enableCallbacks()
{
    setUpdateCallback(new ModelFaceUpdateCallback());
}


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

TriangulatedModel::TriangulatedModel()
{
    m_currentSelection = NULL;

    m_transform = new osg::PositionAttitudeTransform();
    addChild(m_transform.get());

    m_boundingBox.set(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
}

// FIXME
TriangulatedModel::TriangulatedModel(const TriangulatedModel& tm, const osg::CopyOp& copyOp)
    : osg::Group(tm, copyOp)
{
}


// ----------------------------------------------------------------------------
// Scaling factors
// ----------------------------------------------------------------------------

void TriangulatedModel::scalingFactors(double& fx, double& fy, double& fz)
{
    fx = m_boundingBox.xMax() - m_boundingBox.xMin();
    fy = m_boundingBox.yMax() - m_boundingBox.yMin();
    fz = m_boundingBox.zMax() - m_boundingBox.zMin();
}

double TriangulatedModel::scalingFactor()
{
    return m_boundingBox.radius();
}

void TriangulatedModel::rescale()
{
    std::map<unsigned int, osg::ref_ptr<ModelFace> >::iterator it;

    for (it = m_faceMap.begin(); it != m_faceMap.end(); ++it)
        it->second->rescaleControlPoints();
}


// ----------------------------------------------------------------------------
// Attitude handling
// ----------------------------------------------------------------------------

void TriangulatedModel::setAttitude(const osg::Quat& quat)
{
    m_transform->setAttitude(quat);
}

const osg::Quat& TriangulatedModel::attitude() const
{
    return m_transform->getAttitude();
}



// ----------------------------------------------------------------------------
// Face management
// ----------------------------------------------------------------------------

void TriangulatedModel::updateFace(const face_t& face)
{
    m_updateLock.lock();

    face_t *copy = new face_t;

    // copy immediates
    copy->id = face.id;
    copy->num_vertices = face.num_vertices;
    copy->num_triangles = face.num_triangles;
    copy->u_size = face.u_size;
    copy->v_size = face.v_size;

    // allocate arrays
    copy->vertices = new vertex_t[copy->num_vertices];
    copy->triangles =  new triangle_t[copy->num_triangles];
    copy->control_points = new vertex_t[copy->u_size * copy->v_size];

    // copy arrays
    memcpy(copy->vertices, face.vertices, copy->num_vertices * sizeof(vertex_t));
    memcpy(copy->triangles, face.triangles, copy->num_triangles * sizeof(triangle_t));
    memcpy(copy->control_points, face.control_points, copy->u_size * copy->v_size * sizeof(vertex_t));

    m_pendingFaces.push_back(copy);

    m_updateLock.unlock();
}

void TriangulatedModel::processPendingFaces()
{
    // no need to lock if there are no pending faces
    if (m_pendingFaces.empty())
        return;

    m_updateLock.lock();

    while(!m_pendingFaces.empty()) {
        // get the top face off the stack
        face_t* face = m_pendingFaces.back();

        addModelFace(new ModelFace(*face));

        // remove and deallocate the face
        m_pendingFaces.pop_back();
        delete[] face->vertices;
        delete[] face->triangles;
        delete[] face->control_points;
        delete face;
    }

    m_updateLock.unlock();
}


void TriangulatedModel::addModelFace(ModelFace* modelFace)
{
    std::map<unsigned int, osg::ref_ptr<ModelFace> >::iterator it;

    if ((it = m_faceMap.find(modelFace->id())) == m_faceMap.end()) {
        // adding new face
        m_transform->addChild(modelFace);
    } else {
        // deselect faces
        if (m_currentSelection)
            m_currentSelection->setSelectionState(DESELECTED);

        // updating old face
        osg::ref_ptr<ModelFace> oldFace = it->second;

        m_transform->replaceChild(oldFace.get(), modelFace);
        m_faceMap.erase(it);
        m_transform->removeChild(oldFace.get());
        oldFace.release();
    }

    m_faceMap[modelFace->id()] = modelFace;

    // update our bounding box
    m_boundingBox.expandBy(modelFace->geode()->getBoundingBox());
}


void TriangulatedModel::selectFace(unsigned int id)
{
    std::map<unsigned int, osg::ref_ptr<ModelFace> >::iterator it;

    if ((it = m_faceMap.find(id)) != m_faceMap.end()) {
        deselectFace();

        m_currentSelection = it->second.get();
        m_currentSelection->setSelectionState(SELECTED);
    }
}

void TriangulatedModel::deselectFace()
{
    if (m_currentSelection) {
        m_currentSelection->setSelectionState(DESELECTED);
        m_currentSelection = NULL;
    }
}
