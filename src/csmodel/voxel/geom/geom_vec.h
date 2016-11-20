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
 * @file voxel/geom/geom_vec.h
 * defines vectors between geometric points 
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __GEOM_VEC_H__
#define __GEOM_VEC_H__

#include <geom_point.h>

#include <node_vec.h>

/**
 * 0-Vektor.
 */
#define NULL_GEOM_VEC GeomPoint(0, 0, 0)

/**
 * defines vectors between geometric points
 */
class GeomVec : public GeomPoint {
    public:
  
    /**
     * default constructor
     * creates the GeomVec based on the GeomPoint p
     * @param p geometric point
     */
    GeomVec(GeomPoint p);

    /**
     * constructor
     * creates the GeomVec, that represents the vector
     * \f$\over{\mbox{pStart}\ \mbox{pEnd}}\f$ 
     * @param pStart source
     * @param pEnd target
     * @pre pStart.getHeight() == pEnd.getHeight()
     */
    GeomVec(GeomVec pStart, GeomVec pEnd);

    /**
     * returns the determinant of the vectors a, b and c
     * @param a the first vector
     * @param b the second vector
     * @param c the third vector
     * @return the determinant det(a, b, c)
     * @pre a.getHeight() = b.getHeight() = c.getHeight()
     */
    static Coordinate det(GeomVec a, GeomVec b, GeomVec c);

    /**
     * returns the angle fromed by this vector and the vector v,
     * the returned angle is in [0; PI].
     * @param v the second vector
     * @return angle in radians
     * @pre v.getHeight() = getHeight()
     */
    Coordinate getAngle(GeomVec v);

    /**
     * returns the corresponding normalized vector if the vector is not a null 
     * vector, otherwise a null vector is returned
     * @return NULL_GEOM_VEC,            if isNullVec()\n
     *         *this/sqrt(getSqrLength), otherwise
     */
    GeomVec getNormalize();

    /**
     * returns the square of the second norm of the vector
     * @return \f$ \sum_{i= 0}^{\dim - 1} \mbox{getCoordinate(i)}^2 \f$
     */
    Coordinate getSqrLength();

    /**
     * checks, whether this is a null vector
     * @return ABS_VAL(getSqrLength()) < SQR_EPSILON
     */
    bool isNullVec();

    /**
     * returns the cross product of this vector and the vector v
     * @param v the second vector
     * @return CROSS_PRODUCT(*this, v)
     * @pre v.getHeight() = getHeight()
     */
    GeomVec vec_prod(GeomVec v);

    /**
     * returns the geometric vector that if added with this vector 
     * forms a null vector
     * @return NULL_GEOM_VEC - (*this)
     */
    GeomVec operator -();
};

/**
 * returns the resulting difference vector
 * @param p1 the first vector
 * @param p2 the second vector
 * @return p1-p2
 * @pre p1.getHeight() = p2.getHeight()
 */
GeomVec operator-(GeomVec p1, GeomVec p2);

/**
 * returns the resulting sum vector
 * @param p1 the first vector
 * @param p2 the second vector
 * @return p1+p2
 * @pre p1.getHeight() = p2.getHeight()
 */
GeomVec operator+(GeomVec p1, GeomVec p2);

/**
 * returns p multiplied by the scalar r
 * @param r scalar
 * @param p node vector
 * @return r*p
 */
GeomVec operator*(double r, GeomVec p);

/**
 * returns the scalar product of p1 and p2
 * @param p1 the first vector
 * @param p2 the second vector
 * @return <p1; p2>
 * @pre p1.getHeight() = p2.getHeight()
 */
Coordinate operator*(GeomVec p1, GeomVec p2);

#endif // ! __GEOM_VEC_H__
