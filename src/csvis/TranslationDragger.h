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

#ifndef TRANSLATIONDRAGGER_H
#define TRANSLATIONDRAGGER_H


#include "Dragger.h"

enum TranslationDraggerPlane {
    PLANE_NONE = 0,
    PLANE_XY,
    PLANE_XZ,
    PLANE_YZ
};

class TranslationDragger : public Dragger
{
    friend class TransformDragger;

    osg::Matrixd m_lastMatrix;
    osg::Vec3 m_translation;

    osg::Vec3 m_deviceOrigin;

    float m_savedX;
    float m_savedY;

    float m_boundsWidth;
    float m_boundsHeight;
    float m_factor;
    
    TranslationDraggerPlane m_plane;

    void updateBounds();
    void calculateFactor();

    osg::Vec3 mapToPlane(float x, float y);

protected:
    virtual void handleButtonPressed(int button);
    virtual void handleButtonReleased(int button);

    virtual void handleDragBegin();
    virtual void handleDrag();
    virtual void handleDragEnd();

    virtual void createGeometry();

public:
    META_Node(csvis, TranslationDragger);

    // constructor
    TranslationDragger();
    // copy constructor
    TranslationDragger(const TranslationDragger&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    virtual void setPosition(osg::Vec3 position);
    virtual osg::Vec3 position() const;
    
    void restrictToPlane(TranslationDraggerPlane plane);

    virtual bool computeLocalToWorldMatrix(osg::Matrix&, osg::NodeVisitor*) const;
    virtual bool computeWorldToLocalMatrix(osg::Matrix&, osg::NodeVisitor*) const;
};

#endif // TRANSLATIONDRAGGER_H
