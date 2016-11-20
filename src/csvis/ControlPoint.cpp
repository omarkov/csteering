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

#include <osg/ShapeDrawable>

#include "NodeFlags.h"
#include "GUIController.h"
#include "ControlPoint.h"
#include "common.h"


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

ControlPoint::ControlPoint(unsigned int id)
{
    m_id = id;

    createNodes();

    m_selectionSwitch->setSingleChildOn(0);
}

ControlPoint::ControlPoint(const ControlPoint& other, const osg::CopyOp& op)
    : osg::Group(other, op)
{
    m_id = other.m_id;

    createNodes();

    m_selectionSwitch->setSingleChildOn(0);
}

void ControlPoint::createNodes()
{
    // this restricts us to 999+1 control points per face,
    // but I couldn't care less right now
    char name[7];
    snprintf(name, 7, "CP %d", m_id);
    setName(name);

    m_dragger = new TranslationDragger();
    addChild(m_dragger.get());

    // add the switch node
    m_selectionSwitch = new osg::Switch();
    m_dragger->addChild(m_selectionSwitch.get());

    m_selectedGeode = new osg::Geode();
    m_selectedGeode->setName(name);
    m_selectedGeode->setNodeMask(CLICKABLE_FLAG);
    m_selectionSwitch->addChild(m_selectedGeode.get(), false);

    m_deselectedGeode = new osg::Geode();
    m_deselectedGeode->setName(name);
    m_deselectedGeode->setNodeMask(CLICKABLE_FLAG);
    m_selectionSwitch->addChild(m_deselectedGeode.get(), false);

    createGeometry();
}


void ControlPoint::createGeometry()
{
    osg::Vec4 selectedColor(1.0f, 0.0f, 0.0f, 1.0f);
    osg::Vec4 deselectedColor(0.0f, 0.0f, 1.0f, 1.0f);
    osg::Vec3 origin(0.0f, 0.0f, 0.0f);

    // give the size in the symmetrical scaled space
    osg::Vec3 size(0.005f, 0.005f, 0.005f);
    sym2osg(size, size);

    osg::Box *box = new osg::Box(origin, size.x(), size.y(), size.z());

    // selected state
    {
        osg::ShapeDrawable *drawable;
        drawable = new osg::ShapeDrawable(box);
        drawable->setColor(selectedColor);

        m_selectedGeode->addDrawable(drawable);
    }

    // deselected state
    {
        osg::ShapeDrawable *drawable;
        drawable = new osg::ShapeDrawable(box);
        drawable->setColor(deselectedColor);

        m_deselectedGeode->addDrawable(drawable);
    }
}

void ControlPoint::rescale()
{
    m_selectedGeode->removeDrawable(0, m_selectedGeode->getNumDrawables());
    m_deselectedGeode->removeDrawable(0, m_deselectedGeode->getNumDrawables());

    createGeometry();
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ControlPoint::setDraggerDelegate(DraggerDelegate* delegate)
{
    m_dragger->setDelegate(delegate);
}

void ControlPoint::printObjectInfo()
{
    char title[18];
    snprintf(title, sizeof(title), "Control Point %d", id());

    printPositionToObjectInfo(title, m_dragger->position());

    g_GUIController->osd()->showSelectionReticule(m_selectedGeode, title);
}


// ----------------------------------------------------------------------------
// Positioning
// ----------------------------------------------------------------------------

void ControlPoint::setPosition(const osg::Vec3& pos)
{
    m_dragger->setPosition(pos);
}

osg::Vec3 ControlPoint::position() const 
{
    return m_dragger->position();
}


// ----------------------------------------------------------------------------
// Selection
// ----------------------------------------------------------------------------

void ControlPoint::handleSelected()
{
    m_selectionSwitch->setSingleChildOn(1);
    m_dragger->enable();

    printObjectInfo();
}

void ControlPoint::handleDeselected()
{
    m_selectionSwitch->setSingleChildOn(0);
    m_dragger->disable();

    g_GUIController->osd()->hideObjectInfo();
    g_GUIController->osd()->hideSelectionReticule();
}
