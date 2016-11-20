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

#ifndef TRANSFORMDRAGGER_H
#define TRANSFORMDRAGGER_H


#include "Dragger.h"
#include "RotationDragger.h"
#include "TranslationDragger.h"

class TransformDragger : public Dragger, public DraggerDelegate
{
    TranslationDragger* m_translationDragger;
    RotationDragger* m_rotationDragger;
    Dragger* m_activeDragger;

protected:
    virtual void handleButtonPressed(int button);
    virtual void handleButtonReleased(int button);

    virtual void createGeometry();

public:
    META_Node(csvis, TransformDragger);

    // constructor
    TransformDragger();
    // copy constructor
    TransformDragger(const TransformDragger&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);
    ~TransformDragger();
    
    virtual void setPosition(osg::Vec3 position);
    virtual osg::Vec3 position() const;
    virtual void setRotation(osg::Quat rotation);
    virtual osg::Quat rotation() const;

    virtual void drag();

    void setNextDraggerActive();

    virtual bool computeLocalToWorldMatrix(osg::Matrix&, osg::NodeVisitor*) const;
    virtual bool computeWorldToLocalMatrix(osg::Matrix&, osg::NodeVisitor*) const;
    
    /// DraggerDelegate
    virtual void draggerDidBeginDragging();
    virtual void draggerDidMove();
    virtual void draggerDidEndDragging(bool hasChanged);
};

#endif // TRANSFORMDRAGGER_H
