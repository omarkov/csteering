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

#ifndef ROTATIONDRAGGER_H
#define ROTATIONDRAGGER_H

#include "Dragger.h"


class RotationDragger : public Dragger
{
    friend class TransformDragger;

    osg::Quat m_rotation;
    osg::Matrixd m_lastMatrix;

    osg::Quat m_savedRotation;

    osg::Vec3 m_savedClick;
    osg::Vec3 m_savedDrag;

    float m_boundsWidth;
    float m_boundsHeight;

    void updateBounds();

    inline void mapMousePosToSphere(osg::Vec3& vector) const;

protected:
    virtual void handleButtonPressed(int button);
    virtual void handleButtonReleased(int button);

    virtual void handleDragBegin();
    virtual void handleDrag();
    virtual void handleDragEnd();

    virtual void createGeometry();

public:
    META_Node(csvis, RotationDragger);

    // constructor
    RotationDragger();
    // copy constructor
    RotationDragger(const RotationDragger&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    virtual void setRotation(osg::Quat rotation);
    virtual osg::Quat rotation() const;

    virtual bool computeLocalToWorldMatrix(osg::Matrix&, osg::NodeVisitor*) const;
    virtual bool computeWorldToLocalMatrix(osg::Matrix&, osg::NodeVisitor*) const;
};

#endif // ROTATIONDRAGGER_H
