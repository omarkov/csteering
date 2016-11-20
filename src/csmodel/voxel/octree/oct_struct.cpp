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
 * file: voxel/octree/oct_struct.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <octree/oct_struct.h>

#include <iostream>
#include <assert.h>
#include <stdlib.h>

using namespace std;

// ##### OctStruct() #################################################
OctStruct::OctStruct() throw (NotEnoughMemoryException*) : countBorderNodes(0) 
{
    m_root = createOneLeaf();
}

// ##### clear() #####################################################
void OctStruct::clear() {
    assert (exist(getTree()));

    remove(m_root->parts);
}

// ##### canCompact() ################################################
bool OctStruct::canCompact(Node node, Color &color) {
    if (isLeaf(node)) {
        return false;
    }
    
    if (!isLeaf(node.parts[0])) {
        return false;
    }
    
    color = getColor(node.parts[0]);
    
    for (PartType i = 1; i < OCT_PARTS; i++) {
        if (!isLeaf(node.parts[i])) {
            return false;
        }
        
        if (color != getColor(node.parts[i])) {
            return false;
        }
    }
    
    return true;
}

// ##### compact() ###################################################
void OctStruct::compact(Node &node) {
    if (isLeaf(node)) {
        return;
    }
    
    for (PartType i = 0; i < OCT_PARTS; i++) {
        compact(node.parts[i]);
    }
    
    Color color;
    
    if (canCompact(node, color)) {
        compact(node, color);
    }
}

// ##### compact()
void OctStruct::compact(Node &node, Color color) {
    remove(node.parts);
    setLeaf(node);
    setColor(node, color);
}

// ##### createLeaves() ###############################################
OctStruct::_octree OctStruct::createLeaves() throw (NotEnoughMemoryException*) {
    _octree newParts;
    newParts = (_octree)malloc(getNodeSize());
    
    if (newParts == NULL) {
        throw new NotEnoughMemoryException();
    }

    for (PartType i = 0; i < OCT_PARTS; i++) {
        setLeaf(newParts[i]);
        
        #ifdef FILL_SOLIDS
        
        #ifndef CLASSIC_MODE
        
        newParts[i].flag = UNDEF_OBJ;
        
        #else
        
        setColor(newParts[i], NO_OBJECT);
        
        #endif
        
        #else
        
        setColor(newParts[i], NO_OBJECT);
        
        #endif
    }
    
    return newParts;
}

// ##### createOneLeaf() #############################################
OctStruct::_octree OctStruct::createOneLeaf()
    throw (NotEnoughMemoryException*) {
    
    _octree newLeaf = (_octree)malloc(getNodeSize());
    
    if (newLeaf == NULL) {
        throw new NotEnoughMemoryException();
    }

    setLeaf(*newLeaf);
    
    #ifdef FILL_SOLIDS
    
    #ifndef CLASSIC_MODE
    
    newLeaf->flag= UNDEF_OBJ;
    
    #else
    
    setColor(*newLeaf, NO_OBJECT);
    
    #endif
    
    #else
    
    setColor(*newLeaf, NO_OBJECT);
    
    #endif
    
    return newLeaf;
}

// ##### empty() #####################################################
bool OctStruct::empty() {
    assert (exist(getTree()));

    #ifdef AUTOFLUSH
    
    flush();
    
    #endif
    
    if (!isLeaf(*getTree())) {
        return false;
    }
    
    return isNoObject(*getTree());
}

// ##### exist() #####################################################
bool OctStruct::exist(_octree tree) {
    return tree != NULL;
}

// ##### flush() #####################################################
void OctStruct::flush() {
    assert (exist(getTree()));

    compact(*m_root);
}

#ifdef MARK_BORDER
// ##### getBorderColor() ############################################
Color OctStruct::getBorderColor(Node node) {
    Color flagColor = getColor(node);
    return flagColor + MAX_COLOR + 1;
}
#endif

// ##### getColor() ##################################################
Color OctStruct::getColor(Node node) {
    #ifdef AUTOFLUSH
    
    compact(node);
    
    #endif
    
    assert (isLeaf(node));

    return node.flag;
}

// ##### getMaxColor() ###############################################
Color OctStruct::getMaxColor() {
    return 1;
}

// ##### getNodeSize() ###############################################
size_t OctStruct::getNodeSize() {
    return sizeof(Node[OCT_PARTS]);
}

// ##### getTree() ###################################################
OctStruct::_octree& OctStruct::getTree() {
    return m_root;
}

#ifdef MARK_BORDER
// ##### isBorderNode() ##############################################
bool OctStruct::isBorderNode(Node node) {
    return (!isUndefObj(node)) && (node.flag < NO_OBJECT);
}
#endif

// ##### isLeaf() ####################################################
bool OctStruct::isLeaf(Node node) {
    return node.parts == NULL;
}

// ##### isNoObject() ################################################
bool OctStruct::isNoObject(Node node) {
    assert (isLeaf(node));

    return node.flag == NO_OBJECT;
}

// ##### isUndefObj() ################################################
bool OctStruct::isUndefObj(Node node) {
    assert (isLeaf(node));

    return node.flag == UNDEF_OBJ;
}

// ##### getChild() ##################################################
OctStruct::_octree OctStruct::getChild(_octree parent, PartType i) {
    assert (exist(parent));
    assert (!isLeaf(*parent));
    assert (exist(&(parent->parts[i])));
    
    return &(parent->parts[i]);
}

// ##### print() #####################################################
void OctStruct::print() {
    assert (exist(getTree()));

    cout << "Octree: ";
    
    if (empty()) {
        cout << "empty.";
    } else {
        printTree(*getTree());
    }
    
    cout << endl;
}

// ##### printTree() #################################################
void OctStruct::printTree(Node node) {
    if (isLeaf(node)) {
        cout << '+';
        ( isUndefObj(node) ? cout << '?' : cout << getColor(node) );
        return;
    }

    cout << "(";
    
    for (PartType i = 0; i < OCT_PARTS; i++) {
        printTree(node.parts[i]);
    }
    
    cout << ")";
}

// ##### remove() ####################################################
void OctStruct::remove(_octree &subtree) {
    if (subtree == NULL) {
        return;
    }
    
    if (!isLeaf(*subtree)) {
        for (PartType i = 0; i < OCT_PARTS; i++) {
            remove(subtree->parts[i].parts);
        }
    }
    
    free(subtree);
    subtree = NULL;
}

#ifdef MARK_BORDER
// ##### setBorderColor() ############################################
void OctStruct::setBorderColor(Node &node, Color color) {
    assert (color >= NO_OBJECT);
    assert (isLeaf(node));

    node.flag = color - MAX_COLOR - 1;
}
#endif

// ##### setColor() ##################################################
void OctStruct::setColor(Node &node, Color color) {
    assert (color >= NO_OBJECT);
    assert (isLeaf(node));

    node.flag = color;
}

// ##### setLeaf() ###################################################
void OctStruct::setLeaf(Node &node) {
    node.parts = NULL;
}

// ##### setTree() ###################################################
void OctStruct::setTree(_octree &tree) throw (NotEnoughMemoryException*) {
    if (!exist(tree)) {
        m_root = createOneLeaf();
    } else {
        m_root = tree;
    }
}

// ##### stat() ######################################################
void OctStruct::stat(Height h, unsigned &sumNodes, unsigned &leaves, 
                        unsigned &innerNodes, unsigned &borderNodes, 
                        unsigned &normCells) {
    stat(getTree(), h, sumNodes, leaves, innerNodes, normCells);
    borderNodes = countBorderNodes;
}

// ##### stat() 
void OctStruct::stat(_octree tree, Height h, 
                    unsigned &sumNodes, unsigned &leaves, 
                    unsigned &innerNodes, unsigned &normCells) {
  
    if (isLeaf(*tree)) {
        if (getColor(*tree) <= NO_OBJECT) {
            normCells = 0;
        } else {
            normCells = 1 << DIMENSIONS * h;
        }
        
        sumNodes = 1;
        leaves = 1;
        innerNodes = 0;
        return;
    }

    unsigned countSumNodes = 1;
    unsigned countLeaves = 0;
    unsigned countInnerNodes = 1;
    unsigned countNormCells = 0;

    for (PartType i = 0; i < OCT_PARTS; i++) {
        stat(getChild(tree, i), h-1, sumNodes, leaves, innerNodes, 
            normCells);
        countSumNodes += sumNodes;
        countLeaves += leaves;
        countInnerNodes += innerNodes;
        countNormCells += normCells;
    }

    sumNodes = countSumNodes;
    leaves = countLeaves;
    innerNodes = countInnerNodes;
    normCells = countNormCells;
}

// EOF: voxel/octree/oct_struct.cpp
