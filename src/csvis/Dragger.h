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

#ifndef DRAGGER_H
#define DRAGGER_H

#include <osg/MatrixTransform>
#include "DraggerDelegate.h"
#include "InputNotificationMixin.h"
#include "InputDevice.h"


class Dragger : public osg::MatrixTransform, public InputNotificationMixin
{
    friend class TransformDragger;

    bool m_isDragging;

protected:
    InputDevice* m_currentInputDevice;

    bool m_hasChanged;

    DraggerDelegate* m_delegate;
    
    float m_mouseOriginX;
    float m_mouseOriginY;
    float m_mouseDeltaX;
    float m_mouseDeltaY;

    /// called when dragging should begin
    virtual void handleDragBegin() {}
    /// called when currently dragging
    virtual void handleDrag() {}
    /// called when dragging stopped
    virtual void handleDragEnd() {}

    /// Called when the dragger moved
    virtual void handleMove(float x, float y);

    /// Called when it's time to recreate the geometry
    virtual void createGeometry() {}

public:
    META_Node(csvis, Dragger);

    /// constructor
    Dragger();
    /// copy constructor
    Dragger(const Dragger&, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY);

    /// Begin the dragging process
    void beginDrag();
    /// Another dragging frame should happen
    virtual void drag();
    /// End the dragging process
    void endDrag();

    /// Predicate to check if the dragger is currently active
    bool isDragging() { return m_isDragging; }

    /// set the draggers position
    virtual void setPosition(osg::Vec3 position) {}
    /// get the draggers position
    virtual osg::Vec3 position() const { return osg::Vec3(); }
    /// set the draggers rotation
    virtual void setRotation(osg::Quat rotation) {}
    /// get the draggers rotation
    virtual osg::Quat rotation() const { return osg::Quat(); }

    /// Set the delegate object
    void setDelegate(DraggerDelegate* delegate) { m_delegate = delegate; }
    /// Retrieve the delegate (if any)
    DraggerDelegate* delegate() const { return m_delegate; }
};


#endif // DRAGGER_H
