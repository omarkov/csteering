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
 * file: voxel/cad/objects/point.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <point.h>

#include <assert.h>
#include <iostream>

using namespace std;

// ##### Point() #####################################################
Point::Point() {
    assert (DIMENSIONS == 3);

    m_coordinates[X_AXIS] = 0;
    m_coordinates[Y_AXIS] = 0;
    m_coordinates[Z_AXIS] = 0;
}

// ##### Point() #####################################################
Point::Point(Coordinate x, Coordinate y, Coordinate z) {
    assert (DIMENSIONS == 3);

    m_coordinates[X_AXIS] = x;
    m_coordinates[Y_AXIS] = y;
    m_coordinates[Z_AXIS] = z;
}

// ##### Point() #####################################################
Point::Point(const Point& point){
	assert (DIMENSIONS == 3);

    m_coordinates[X_AXIS] = point.m_coordinates[X_AXIS];
    m_coordinates[Y_AXIS] = point.m_coordinates[Y_AXIS];
    m_coordinates[Z_AXIS] = point.m_coordinates[Z_AXIS];
}

// ##### getCoordinate() #############################################
Coordinate Point::getCoordinate(Axis axis) {
    assert (axis < DIMENSIONS);

    return m_coordinates[axis];
}

// ##### getDataPoint() ##############################################
const CadObject::DataType Point::getDataType() {
    return CadObject::POINT;
}

// ##### getDimension() ##############################################
int Point::getDimension() {
    return 0;
}

// ##### getMaxPoint() ###############################################
Point Point::getMaxPoint() {
    return Point(getX(), getY(), getZ());
}

// ##### getMinPoint() ###############################################
Point Point::getMinPoint() {
    return getMaxPoint();
}

// ##### getX() ######################################################
Coordinate Point::getX() {
    return getCoordinate(X_AXIS);
}

// ##### getY() ######################################################
Coordinate Point::getY() {
    return getCoordinate(Y_AXIS);
}

// ##### getZ() ######################################################
Coordinate Point::getZ() {
    return getCoordinate(Z_AXIS);
}

// ##### print() #####################################################
void Point::print() {
    cout << "(" << getCoordinate(0);
    
    for (Axis axis = 1; axis < DIMENSIONS; axis++) {
        cout << "; " << getCoordinate(axis);
    }
  cout << ")";
}

// ##### set() #######################################################
void Point::set(Coordinate x, Coordinate y, Coordinate z) {
    assert (DIMENSIONS == 3);

    setX(x);
    setY(y);
    setZ(z);
}

// ##### setCoordinate() #############################################
void Point::setCoordinate(Axis axis, Coordinate value) {
    assert (axis < DIMENSIONS);

    m_coordinates[axis] = value;
}

// ##### setX ########################################################
void Point::setX(Coordinate value) {
    return setCoordinate(X_AXIS, value);
}

// ##### setY ########################################################
void Point::setY(Coordinate value) {
    return setCoordinate(Y_AXIS, value);
}

// ##### setZ ########################################################
void Point::setZ(Coordinate value) {
    return setCoordinate(Z_AXIS, value);
}

// ##### operator[] ##################################################
Coordinate Point::operator[](Axis axis) {
    return getCoordinate(axis);
}

// ##### operator== ##################################################
bool operator==(Point p1, Point p2) {
    for (Axis axis = 0; axis < DIMENSIONS; axis++) {
        if (!EQUIVAL(p1[axis], p2[axis])) {
            return false;
        }
    }
    return true;
}

// ##### operator< ###################################################
bool operator<(Point p1, Point p2) {
    for (Axis axis = 0; axis < DIMENSIONS; axis++) {    
        if (p1[axis] < p2[axis]) {
            return true;
        }
    
        if (p1[axis] > p2[axis]) {
            return false;
        }
    }
    return false;
}

// ##### operator<< ##################################################
bool operator<<(Point p1, Point p2) {
    for (Axis axis= 0; axis < DIMENSIONS; axis++) {
        if ((p1[axis] > p2[axis]) || EQUIVAL(p1[axis], p2[axis])) {
            return false;
        }
    }
    return true;
}

// EOF: voxel/cad/objects/point.cpp
