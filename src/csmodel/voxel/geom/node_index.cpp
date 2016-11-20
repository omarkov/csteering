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
 * file: voxel/geom/node_index.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <node_index.h>

#include <geom_point.h>
#include <polygon.h>

#include <assert.h>
#include <iostream>

using namespace std;

// ##### NodeIndex() #################################################
NodeIndex::NodeIndex(AxIndex x, AxIndex y, AxIndex z, Height h) {
    assert (DIMENSIONS == 3);

    m_indices[X_AXIS] = x;
    m_indices[Y_AXIS] = y;
    m_indices[Z_AXIS] = z;

    m_height = h;
}

// ##### getAxCoordinate() ###########################################
AxIndex NodeIndex::getCoordinate(Axis axis) {
    assert (axis < DIMENSIONS);

    return m_indices[axis];
}

// ##### getHeight() ##################################################
Height NodeIndex::getHeight() {
    return m_height;
}

// ##### getX() ######################################################
AxIndex NodeIndex::getX() {
    return getCoordinate(X_AXIS);
}

// ##### getY() ######################################################
AxIndex NodeIndex::getY() {
    return getCoordinate(Y_AXIS);
}

// ##### getZ() ######################################################
AxIndex NodeIndex::getZ() {
    return getCoordinate(Z_AXIS);
}

// ##### print() #####################################################
void NodeIndex::print() {
    cout << "(" << getCoordinate(0);
  
    for (Axis axis = 1; axis < DIMENSIONS; axis++) {
        cout << "; " << getCoordinate(axis);
    }
    
    cout << "| " << m_height << ")";
}

// ##### set() #######################################################
void NodeIndex::set(AxIndex x, AxIndex y, AxIndex z) {
    assert (DIMENSIONS == 3);

    setX(x);
    setY(y);
    setZ(z);
}

// ##### setCoordinate() #############################################
void NodeIndex::setCoordinate(Axis axis, AxIndex value) {
    assert (axis < DIMENSIONS);

    m_indices[axis] = value;
}

// ##### setHeight() ##################################################
void NodeIndex::setHeight(Height h) {
    
    if (h < getHeight()) {
        // deeper
        Height dh = getHeight() - h;
        set(getX() << dh, getY() << dh, getZ() << dh);
    } else {
        // higher
        Height dh = h - getHeight();
        set(getX() >> dh, getY() >> dh, getZ() >> dh);
    }
  
    m_height = h;
}

// ##### setX() ######################################################
void NodeIndex::setX(AxIndex value) {
    setCoordinate(X_AXIS, value);
}

// ##### setY() ######################################################
void NodeIndex::setY(AxIndex value) {
    setCoordinate(Y_AXIS, value);
}

// ##### setZ() ######################################################
void NodeIndex::setZ(AxIndex value) {
    setCoordinate(Z_AXIS, value);
}

// ##### operator[] ##################################################
AxIndex NodeIndex::operator[](Axis axis) {
    return getCoordinate(axis);
}

// ##### operator== ##################################################
bool operator==(NodeIndex p1, NodeIndex p2) {
    assert (p1.getHeight() == p2.getHeight());

    for (Axis axis = 0; axis < DIMENSIONS; axis++) {
        if (p1[axis] != p2[axis]) {
            return false;
        }
    }
    
    return true;
}

// ##### operator< ###################################################
bool operator<(NodeIndex p1, NodeIndex p2) {
    assert (p1.getHeight() == p2.getHeight());

    for (Axis axis = 0; axis < DIMENSIONS; axis++) {
        if (p1[axis] != p2[axis]) {
            return p1[axis] < p2[axis];
        }
    }
  
    return false;
}

// EOF: voxel/geom/node_index.cpp
