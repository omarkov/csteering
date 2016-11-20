// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Mikhail Prokharau, StuPro A - CS 
//  <csteering-devel@duck.informatik.uni-stuttgart.de>
//  All rights reserved.
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

/**
 * file: voxel/octree/index_oct.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <octree/index_oct.h>

#include <generator/oct_gen.h>

#include <assert.h>

// ##### IndexOct() ##################################################
IndexOct::IndexOct(Height maxTreeHeight) throw(NotEnoughMemoryException*)
  : OctStruct(), m_rootHeight(maxTreeHeight) {
    m_minPoint = Point();
    m_maxPoint = Point();
    m_minGeomPoint = GeomPoint();
    m_maxGeomPoint = GeomPoint();
}

// ##### IndexOct() ##################################################
IndexOct::IndexOct(Height maxTreeHeight, Point minPoint, Point maxPoint,
    GeomPoint minGeomPoint, GeomPoint maxGeomPoint) 
        throw(NotEnoughMemoryException*)
            : OctStruct(), m_rootHeight(maxTreeHeight) {
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_minGeomPoint = minGeomPoint;
    m_maxGeomPoint = maxGeomPoint;
}

// ##### add() #######################################################
void IndexOct::add(NodeIndex p, Color color)
    throw (NotEnoughMemoryException*) {
  
    _octree subtree;
    Height h = getExistNode(p, subtree);

    assert (subtree != NULL);
  
    if (isIn(p, h) && !OctStruct::isLeaf(*subtree)) {
        assert (false);
        return;
    }

    while (h > p.getHeight()) {
        h--;
        subtree->parts = createLeaves();
        subtree = OctStruct::getChild(subtree, getPart(p, h - p.getHeight()));
    }

    if (OctStruct::getColor(*subtree) != color) {
        OctStruct::setColor(*subtree, color);
        countBorderNodes++;
    }
}

// ##### add() 
void IndexOct::add(Surface *s, Color color)
    throw (NotEnoughMemoryException*) {
    
    assert (s != NULL);
    assert (s->getHeight() == BASE_NODE_HEIGHT);

    add(getTree(), NodeIndex(0, 0, 0, getMaxTreeHeight()), s, color);
}

// ##### add() 
void IndexOct::add(_octree subtree, NodeIndex myIdx, Surface* s, Color color)
    throw (NotEnoughMemoryException*) {
    
    assert (isIn(myIdx));

    if (!s->isIn(GeomPoint(myIdx))) {
        return;
    }
    
    if (myIdx.getHeight() == BASE_NODE_HEIGHT) {
        OctStruct::setColor(*subtree, color);
        return;
    }

    if (OctStruct::isLeaf(*subtree)) {
        subtree->parts= createLeaves();
    }
    
    for (PartType i= 0; i < OCT_PARTS; i++) {
        add(OctStruct::getChild(subtree, i), getChild(myIdx, i), s, color);
    }
}

// ##### exist() #####################################################
bool IndexOct::exist(NodeIndex p) {
    _octree subtree;
    Height h = getExistNode(p, subtree);
    return isIn(p, h);
}

// ##### getChild() ##################################################
NodeIndex IndexOct::getChild(NodeIndex parent, PartType i) {
    assert (parent.getHeight() != BASE_NODE_HEIGHT);
    assert (isIn(parent));

    return NodeIndex((parent.getX() << 1) + getPartOfs(i, X_AXIS),
                     (parent.getY() << 1) + getPartOfs(i, Y_AXIS),
                     (parent.getZ() << 1) + getPartOfs(i, Z_AXIS),
                      parent.getHeight() - 1);
}

// ##### getColor() ##################################################
Color IndexOct::getColor(NodeIndex p) {
    Node* nodePtr;
    getExistNode(p, nodePtr);
    return OctStruct::getColor(*nodePtr);
}

// ##### getExistNode() ##############################################
Height IndexOct::getExistNode(NodeIndex p, _octree &subtree) {
    assert (isIn(p, p.getHeight()));
    assert (OctStruct::exist(getTree()));

    subtree = getTree();
    Height h = getMaxTreeHeight();
    
    while (h > p.getHeight() && !OctStruct::isLeaf(*subtree)) {
        h--;
        subtree = OctStruct::getChild(subtree, getPart(p, h - p.getHeight()));
    }
  
    assert (OctStruct::exist(subtree));
    assert (0 <= h && h <= getMaxTreeHeight());
    
    return h;
}

// ##### getExistNode() 
NodeIndex IndexOct::getExistNode(NodeIndex p) {
    assert (0<= p.getHeight() && p.getHeight() <= getMaxTreeHeight());

    _octree tree;
    Height h= getExistNode(p, tree);

    NodeIndex idx = p;
    idx.setHeight(h);

    if (h > p.getHeight() || h == 0) {
        assert(OctStruct::isLeaf(*tree));
    }
    
    return idx;
}

// ##### getMaxTreeHeight() ###########################################
Height IndexOct::getMaxTreeHeight() {
    return m_rootHeight;
}

// ##### getPart() ###################################################
PartType IndexOct::getPart(AxIndex idx, Height h) {
    return (idx >> h) % AXIS_PARTS;
}

// ##### getPart() 
PartType IndexOct::getPart(NodeIndex p, Height h) {
    PartType part= 0;
    
    for (Axis axis = DIMENSIONS - 1; axis >= 0; axis--) {
        part = AXIS_PARTS * part + getPart(p[axis], h);
    }
    
    assert (part < OCT_PARTS);
    
    return part;
}

// ##### getPartOfs() ################################################
AxIndex IndexOct::getPartOfs(PartType partNumber, Axis axis) {
    return (partNumber >> axis) % AXIS_PARTS;
}

// ##### getMinPoint() ###############################################
Point IndexOct::getMinPoint(){
    return m_minPoint;
};
  
// ##### getMaxPoint() ###############################################
Point IndexOct::getMaxPoint(){
    return m_maxPoint;
};
  
// ##### getMinGeomPoint() ###########################################  
GeomPoint IndexOct::getMinGeomPoint(){
    return m_minGeomPoint;
};
  
// ##### getMaxGeomPoint() ##########################################  
GeomPoint IndexOct::getMaxGeomPoint(){
    return m_maxGeomPoint;
};

// ##### isIn() ######################################################
bool IndexOct::isIn(NodeIndex p) {
    if (p.getHeight() > getMaxTreeHeight()) {
        return false;
    }
  
    for (Axis axis = 0; axis < DIMENSIONS; axis++) {
        if (p[axis] < 0 ||
            ((p[axis] >> (getMaxTreeHeight() - p.getHeight())) >= 1) ) {
            return false;
        }
    }
    
    return true;
}

// ##### isIn() ######################################################
bool IndexOct::isIn(NodeIndex p, Height h) {
    return p.getHeight() <= h && h <= getMaxTreeHeight() && isIn(p);
}

// ##### isLeaf() ####################################################
bool IndexOct::isLeaf(NodeIndex p) {
    Node* nodePtr;
    
    #ifdef NDEBUG
    
    getExistNode(p, nodePtr);
    
    #else
    
    Height h = getExistNode(p, nodePtr);
    
    assert (isIn(p, h));
    
    #endif

    return OctStruct::isLeaf(*nodePtr);
}

#ifdef MARK_BORDER
// ##### setBorderColor() ############################################
void IndexOct::setBorderColor(NodeIndex p, Color color) {
    Node* nodePtr;
    getExistNode(p, nodePtr);

    assert (OctStruct::isLeaf(*nodePtr));

    OctStruct::setBorderColor(*nodePtr, color);
}
#endif

// ##### setMinPoint() ###############################################
void IndexOct::setMinPoint(Point minPoint){
    m_minPoint = minPoint;
};
  
// ##### setMaxPoint() ###############################################
void IndexOct::setMaxPoint(Point maxPoint){
    m_maxPoint = maxPoint;
};

// ##### setMinGeomPoint() ###########################################
void IndexOct::setMinGeomPoint(GeomPoint minGeomPoint){
    m_minGeomPoint = minGeomPoint;
};
  
// ##### setMaxGeomPoint() ###########################################
void IndexOct::setMaxGeomPoint(GeomPoint maxGeomPoint){
    m_maxGeomPoint = maxGeomPoint;
};

// ##### setColor() ##################################################
void IndexOct::setColor(NodeIndex p, Color color) {
    Node* nodePtr;
    getExistNode(p, nodePtr);

    assert (OctStruct::isLeaf(*nodePtr));

    OctStruct::setColor(*nodePtr, color);
}

// EOF: voxel/octree/index_oct.cpp
