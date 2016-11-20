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
#include <iostream>
#include <osgUtil/TriStripVisitor>

#include "GUIController.h"
#include "NodeFlags.h"
#include "ModelFace.h"


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

ModelFace::ModelFace(const face_t& face)
{
    m_faceID = face.id;

    m_controlPoints = new ControlPointGrid(face);
    m_controlPoints->setNodeMask(0x0);

    m_selectionSwitch = new osg::Switch();

    createNodes(face);

    m_selectionSwitch->addChild(m_controlPoints.get());
    m_selectionSwitch->setAllChildrenOff();

    addChild(m_selectionSwitch.get());
}

// FIXME
ModelFace::ModelFace(const ModelFace& other, const osg::CopyOp& copyOp)
{
}


void ModelFace::rescaleControlPoints()
{
    m_controlPoints->rescale();
}

// ----------------------------------------------------------------------------
// Node creation
// ----------------------------------------------------------------------------

void ModelFace::createNodes(const face_t& face)
{
    m_faceGeode = new osg::Geode();
    m_faceGeode->setNodeMask(CLICKABLE_FLAG);

    osg::Geometry* geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array(face.num_vertices);
    osg::UIntArray* indices = new osg::UIntArray(face.num_triangles * 3);

    // copy vertices
    for (unsigned int i = 0; i < face.num_vertices; i++)
        (*vertices)[i].set(face.vertices[i].x, face.vertices[i].y, face.vertices[i].z);

    // copy indices
    for (unsigned int i = 0; i < face.num_triangles; i++) {
        (*indices)[i*3] = face.triangles[i].a;
        (*indices)[i*3+1] = face.triangles[i].b;
        (*indices)[i*3+2] = face.triangles[i].c;
    }

    geometry->setVertexArray(vertices);
    geometry->setVertexIndices(indices);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,
                                                  0,
                                                  face.num_triangles *3));

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    calculateVertexNormals(geometry);

    /*
    // make tri strips
    osgUtil::TriStripVisitor tsv;
    tsv.stripify(*geometry);
    */

    m_faceGeode->addDrawable(geometry);

    // set a name
    char num[5];
    snprintf(num, 5, "%d", face.id);
    m_faceGeode->setName(std::string("Face ") + std::string(num));

    addChild(m_faceGeode.get());
}



// ----------------------------------------------------------------------------
// Normal calculation
// ----------------------------------------------------------------------------

osg::Vec3Array* ModelFace::calculateSurfaceNormals(osg::Geometry* geometry)
{
    assert(geometry);

    // get vertices and triangles
    osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
    osg::UIntArray* indices = dynamic_cast<osg::UIntArray*>(geometry->getVertexIndices());

    osg::Vec3Array* normals = new osg::Vec3Array();

    unsigned int num_triangles = indices->size() / 3;
    unsigned int i, j;

    for (i = 0, j = 0; i < num_triangles; i++, j+=3) {
        // (v1 - v2) x (v2 - v3)
        osg::Vec3 normal = ((*vertices)[(*indices)[j]] - (*vertices)[(*indices)[j+1]])
                           ^ ((*vertices)[(*indices)[j+1]] - (*vertices)[(*indices)[j+2]]);

        normal.normalize();
        normals->push_back(normal);
    }

    return normals;
}

// i think this may qualify as the ugliest code imaginable
void ModelFace::calculateVertexNormals(osg::Geometry* geometry)
{
    assert(geometry);

    // get vertices and triangles
    osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
    osg::UIntArray* indices = dynamic_cast<osg::UIntArray*>(geometry->getVertexIndices());
    unsigned int num_triangles = indices->size() / 3;
    unsigned int i, j, k;

    // calculate surface normals
    osg::ref_ptr<osg::Vec3Array> surface_normals = calculateSurfaceNormals(geometry);

    // calculate vertex normals
    osg::Vec3Array *vertex_normals = new osg::Vec3Array;

    for (i = 0; i < vertices->size(); i++) {
        osg::Vec3 sum;

        for (j = 0, k = 0; j < num_triangles; j++, k+=3) {
            if (i == (*indices)[k] || i == (*indices)[k+1] || i == (*indices)[k+2])
                sum += (*surface_normals)[j];
        }
        sum.normalize();
        vertex_normals->push_back(sum);
    }

    // set normals
    geometry->setNormalArray(vertex_normals);
    geometry->setNormalIndices(indices);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
}


// ----------------------------------------------------------------------------
// Selection
// ----------------------------------------------------------------------------

void ModelFace::handleSelected()
{
    // show the control points
    m_selectionSwitch->setSingleChildOn(0);
    m_controlPoints->setNodeMask(0xffffffff);

    // set the color of the face to a light red
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 0.7f, 0.7f, 1.0f));

    osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(m_faceGeode->getDrawable(0));

    if (geometry) {
        geometry->setColorArray(colors);
        geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    }

    // show the selection reticule
    g_GUIController->osd()->showSelectionReticule(m_faceGeode, m_faceGeode->getName());
}

void ModelFace::handleDeselected()
{
    m_controlPoints->deselectAll();
    m_controlPoints->setNodeMask(0x0);

    // hide the control points
    m_selectionSwitch->setAllChildrenOff();

    // set the color of the face to white
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(m_faceGeode->getDrawable(0));

    if (geometry) {
        geometry->setColorArray(colors);
        geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    }

    // hide the selection reticule
    g_GUIController->osd()->hideSelectionReticule();
}


void ModelFace::selectControlPoint(unsigned int index)
{
    m_controlPoints->selectControlPoint(index);
};
