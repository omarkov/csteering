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
 * @file voxel/geom/line.h
 * defines a line passing through two points
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __LINE_H__
#define __LINE_H__

#include <geom_vec.h>

/**
 * line passing through two points
 */
class Line {
    public:
  
    /**
     * constructor
     * @param pA the first point
     * @param pB the second point
     * @pre pA.getHeight() = pB.getHeight()
     */
    Line(GeomPoint pA, GeomPoint pB);

    /**
     * returns the number of the end points
     * @return 2
     */
    int getCount();

    /**
     * returns the direction vector
     * @return pB - pA
     */
    GeomVec getDirVec();

    /**
     * returns the distance vector from p to the root point
     * @param p point
     * @return distance vector
     * @pre p.getHeight() = getHeight()
     */
    GeomVec getDistVec(GeomPoint p);

    /**
     * returns the root point of p on the line,
     * the root point is the point on the line whose distance to p is 
     * the smallest
     * @param p point
     * @return the root point of p
     */
    GeomPoint getFootpoint(GeomPoint p);

    /**
     * returns the virtual height of the line
     * @return the height of the line
     */
    Height getHeight();

    /**
     * returns the idx-th end point
     * @param idx index of the point
     * @return endpoint no. idx
     * @pre 0 <= idx < 2
     */
    GeomPoint getPoint(int idx);

    /**
     * checks whether p lies on the same line that is defined by this line
     * @param p point
     * @return p lies on the same line or not
     * @pre p.getHeight() = getHeight()
     */
    bool isAtLine(GeomPoint p);

    /**
     * checks whether p is the end point of this line
     * @return p is the end point of this line or not
     * @pre p.getHeight() = getHeight()
     */
    bool isEndpoint(GeomPoint p);

    /**
     * checks whether p lies between the two points defining the line
     * @param p point
     * @return p lies between the two points or not
     * @pre p.getHeight() = getHeight()
     */
    bool isInStretch(GeomPoint p);

    /**
     * prints the line to the console
     */
    void print();

    /**
     * sets the virtual height of the line
     * @param h height of the line
     */
    void setHeight(Height h);

    /**
     * returns the idx-th point
     * @param idx the number of the point
     * @return corner point no. idx
     * @pre 0 <= idx < 2
     */
    GeomPoint operator[](int idx);

    private:
  
    /**
     * point A
     */
    GeomPoint m_pointA;

    /**
     * point B
     */
    GeomPoint m_pointB;
}; // class Line

#endif // ! __LINE_H__
