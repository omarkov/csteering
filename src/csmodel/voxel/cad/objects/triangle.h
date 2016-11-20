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
 * @file voxel/cad/objects/triangle.h
 * defines the type Triangle
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <plane.h>
#include <point.h>

/**
 * concrete class for a triangle
 */
class Triangle : public Plane {
    
    public:
    
    /**
     * constructor
     * @param pA the first corner point
     * @param pB the second corner point
     * @param pC the third corner point
     */
    Triangle(Point pA, Point pB, Point pC);

    /**
     * returns the number of the corner points
     * @return 3
     */
    virtual int getCornerCount();

    /**
     * returns the type of the CAD object
     * @return CadObject::TRIANGLE
     */
    virtual const DataType getDataType();

    /**
     * returns the idx-th corner point
     * @param idx no. of the corner point
     * @return corner point no. idx
     * @pre 0 <= idx < 3
     */
    Point operator[](int idx);

    private:
  
    /**
     * corner point A
     */
    Point m_pointA;

    /**
     * corner point B
     */
    Point m_pointB;

    /**
     * corner point C
     */
    Point m_pointC;
}; // class Triangle

#endif // ! __TRIANGLE_H__
