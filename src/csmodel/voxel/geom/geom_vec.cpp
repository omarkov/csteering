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
 * file: voxel/geom/geom_vec.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <geom_vec.h>
#include <global.h>

#include <math.h>
#include <assert.h>
#include <iostream>

// ##### GeomVec() ###################################################
GeomVec::GeomVec(GeomPoint p) : GeomPoint(p) {}

// ##### GeomVec() 
GeomVec::GeomVec(GeomVec pStart, GeomVec pEnd) : GeomPoint(pEnd - pStart) {
    assert (pStart.getHeight() == pEnd.getHeight());
}

// ##### det() #######################################################
Coordinate GeomVec::det(GeomVec a, GeomVec b, GeomVec c) {
    assert (a.getHeight() == b.getHeight() && b.getHeight() == c.getHeight());

    // | a_x b_x c_x | a_x  b_x
    // | a_y b_y c_y | a_y  b_y
    // | a_z b_z c_z | a_z  b_z
    return a[X_AXIS] * (b[Y_AXIS]*c[Z_AXIS] - c[Y_AXIS]*b[Z_AXIS])
         + b[X_AXIS] * (c[Y_AXIS]*a[Z_AXIS] - a[Y_AXIS]*c[Z_AXIS])
         + c[X_AXIS] * (a[Y_AXIS]*b[Z_AXIS] - b[Y_AXIS]*a[Z_AXIS]);
}

// ##### getAngle() ##################################################
Coordinate GeomVec::getAngle(GeomVec v) {
    assert (v.getHeight() == getHeight());

    return acos(getNormalize() * v.getNormalize());
}

// ##### getNormalize() ##############################################
GeomVec GeomVec::getNormalize() {
    Coordinate length = sqrt(getSqrLength());
    
    if (EQUIVAL(length, 0.0)) {
        GeomVec v = NULL_GEOM_VEC;
        v.setHeight(getHeight());
        return v;
    }
  
    return (1.0 / length) * (*this);
}

// ##### getSqrLength() ##############################################
Coordinate GeomVec::getSqrLength() {
    return (*this) * (*this);
}

// ##### isNullVec() ################################################
bool GeomVec::isNullVec() {
    return ABS_VAL(getSqrLength()) < VEC_ABS_MAX + EPSILON;
}

// ##### vec_prod() ##################################################
GeomVec GeomVec::vec_prod(GeomVec v) {
    assert (v.getHeight() == getHeight());
  
    // | e_x e_y e_z |
    // | t_x t_y t_z |
    // | v_x v_y v_z |
    return GeomPoint(getY()*v[Z_AXIS] - getZ()*v[Y_AXIS],
                     getZ()*v[X_AXIS] - getX()*v[Z_AXIS], 
                     getX()*v[Y_AXIS] - getY()*v[X_AXIS], 
                     getHeight());
}

// ##### operator- ###################################################
GeomVec GeomVec::operator-() {
    return GeomPoint(-getX(), -getY(), -getZ(), getHeight());
}

// ##### operator- 
GeomVec operator-(GeomVec p1, GeomVec p2) {
    return p1 + (-p2);
}

// ##### operator+ ###################################################
GeomVec operator+(GeomVec p1, GeomVec p2) {
    assert (p1.getHeight() == p2.getHeight());

    return GeomPoint
        ( p1.getX() + p2.getX(), p1.getY() + p2.getY(), p1.getZ() + p2.getZ(),
          p1.getHeight() );
}

// ##### operator* ###################################################
GeomVec operator*(double r, GeomVec p) {
    return GeomPoint(r * p.getX(), r * p.getY(), r * p.getZ(), p.getHeight());
}

// ##### operator* ###################################################
Coordinate operator*(GeomVec p1, GeomVec p2) {
    assert (p1.getHeight() == p2.getHeight());

    return p1.getX() * p2.getX() + p1.getY() * p2.getY() 
         + p1.getZ() * p2.getZ();
}

// EOF: voxel/geom/geom_vec.cpp
