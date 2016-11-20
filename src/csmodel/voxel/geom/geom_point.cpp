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
 * file: voxel/geom/geom_point.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <geom_point.h>

#include <geom_vec.h>
#include <octree/index_oct.h>

#include <assert.h>
#include <iostream>

using namespace std;

// ##### GeomPoint() #################################################
GeomPoint::GeomPoint(Coordinate x, Coordinate y, Coordinate z, Height h)
    : m_indexHeight(h) {
    set(x, y, z);
}

// ##### GeomPoint() 
GeomPoint::GeomPoint(NodeIndex p) : m_indexHeight(p.getHeight()) {
    set(p.getX() + OFS, p.getY() + OFS, p.getZ() + OFS);
}

// ##### ccw() #######################################################
int GeomPoint::ccw(GeomPoint p1, GeomPoint p2, Axis proj) {
    Axis i = (proj + 1) % DIMENSIONS;
    Axis k = (proj + 2) % DIMENSIONS;

    GeomVec lP1 = GeomVec(*this, p1);
    GeomVec lP2 = GeomVec(*this, p2);
    
    if (lP1[i] * lP2[k] > lP1[k] * lP2[i]) return +1;
    
    if (lP1[i] * lP2[k] < lP1[k] * lP2[i]) return -1;

    return 0;
}

// ##### getAxIndex() ################################################
AxIndex GeomPoint::getAxIndex(Axis axis) {
    assert (axis < DIMENSIONS);

    Coordinate coord = m_coordinates[axis];
    AxIndex axIdx = static_cast<AxIndex>(coord);

    return axIdx;
}

// ##### getCoordinate() #############################################
Coordinate GeomPoint::getCoordinate(Axis axis) {
    assert (axis < DIMENSIONS);

    return m_coordinates[axis];
}

// ##### getDup() ####################################################
GeomPoint GeomPoint::getDup() {
    return GeomPoint(getX(), getY(), getZ(), getHeight());
}

// ##### getHeight() ##################################################
Height GeomPoint::getHeight() {
    return m_indexHeight;
}

// ##### getNodeIndex() ##############################################
NodeIndex GeomPoint::getNodeIndex() {
    return NodeIndex(getAxIndex(X_AXIS), getAxIndex(Y_AXIS), getAxIndex(Z_AXIS), 
        getHeight());
}

// ##### getX() ######################################################
Coordinate GeomPoint::getX() {
    return getCoordinate(X_AXIS);
}

// ##### getY() ######################################################
Coordinate GeomPoint::getY() {
    return getCoordinate(Y_AXIS);
}

// ##### getZ() ######################################################
Coordinate GeomPoint::getZ() {
    return getCoordinate(Z_AXIS);
}

// ##### print() #####################################################
void GeomPoint::print() {
    cout << "(" << getCoordinate(0);
  
    for (Axis axis = 1; axis < DIMENSIONS; ++axis) {
        cout << "; " << getCoordinate(axis);
    }
    
    cout << ")";
}

// ##### set() #######################################################
void GeomPoint::set(Coordinate x, Coordinate y, Coordinate z) {
    assert (DIMENSIONS == 3);

    setX(x);
    setY(y);
    setZ(z);
}

// ##### setCoordinate() #############################################
void GeomPoint::setCoordinate(Axis axis, Coordinate value) {
    assert (axis < DIMENSIONS);

    m_coordinates[axis]= value;
}

// ##### setHeight() ##################################################
void GeomPoint::setHeight(Height h) {
    Height oldHeight = getHeight();
    
    if (h == oldHeight) {
        return;
    }

    Coordinate dh = (Coordinate)(1 << (Height)ABS_VAL(h - oldHeight));
    Coordinate ofs= 0.0;
    
    if (oldHeight < h) {
        set((getX() - ofs) / dh + ofs, (getY() - ofs) / dh + ofs, 
            (getZ() - ofs) / dh + ofs);
    } else {
        set(dh * (getX() - ofs) + ofs, dh * (getY() - ofs) + ofs, 
            dh * (getZ() - ofs) + ofs);
    }
  
    m_indexHeight = h;
}

// ##### setX ########################################################
void GeomPoint::setX(Coordinate value) {
    return setCoordinate(X_AXIS, value);
}

// ##### setY ########################################################
void GeomPoint::setY(Coordinate value) {
    return setCoordinate(Y_AXIS, value);
}

// ##### setZ ########################################################
void GeomPoint::setZ(Coordinate value) {
    return setCoordinate(Z_AXIS, value);
}

// ##### operator[] ##################################################
Coordinate GeomPoint::operator[](Axis axis) {
    return getCoordinate(axis);
}

// ##### operator== ##################################################
bool operator==(GeomPoint p1, GeomPoint p2) {
    assert (p1.getHeight() == p2.getHeight());

    return GeomVec(p1, p2).isNullVec();
}

// EOF: voxel/geom/geom_point.cpp
