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
 * @file voxel/geom/polygon.h
 * polygon consisting of geometric points
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <surface.h>
#include <geom_point.h>
#include <line.h>

/**
 * polygon defined by geometric points 
 */
class Polygon : public Surface {
    public:
  
    /**
     * constructor, creates a triangle
     * @param pA the first corner point
     * @param pB the second corner point
     * @param pC the third corner point
     */
    Polygon(GeomPoint pA, GeomPoint pB, GeomPoint pC);

    /**
     * constructor, creates a quadrilateral
     * @param pA the first corner point
     * @param pB the second corner point
     * @param pC the third corner point
     * @param pD the fourth corner point
     * @pre all point must be on the same plane
     */
    Polygon(GeomPoint pA, GeomPoint pB, GeomPoint pC, GeomPoint pD);

    /**
     * returns the number of the corner points
     * @return \em 3, for triangles und \em 4, for quadrilaterals
     */
    int getCount();

    /**
     * returns a copy of the polygon
     * @return copy
     */
    Polygon getDup();

    /**
     * returns the virtual height of the polygon in the octal tree
     * @return height of the polygon in the octal tree
     */
    virtual Height getHeight();

    /**
     * returns the root point of p lying on the plane
     * @param p point
     * @return root point of p on the plane (p, if p is on the plane)
     */
    GeomPoint getFootpoint(GeomPoint p);

    /**
     * returns the normalized normal vector to the plane,
     * the vector is perpendicular to the plane and has the absolute value of 1,
     * if the plane is actually a plane, otherwise (if it is a point or a line),
     * a null vector is returned
     * @return normalized normal vector
     */
    GeomVec getNormVec();

    /**
     * returns the idx-th corner point
     * @param idx number of the corner point
     * @return corner point no. idx
     * @pre 0 <= idx < getCount()
     */
    GeomPoint& getPoint(int idx);

    /**
     * checks whether p is in the polygon
     * @param p point
     * @return p is in the polygon or not
     */
    virtual bool isIn(GeomPoint p);

    /**
     * checks whether p lies on the plane formed by the first three 
     * corner points of the polygon
     * @param p point
     * @return p lies on the plane formed by the first three 
     * corner points of the polygon or not
     */
    bool isInPlane(GeomPoint p);

    /**
     * prints this poygon to the console
     */
    void print();

    /**
     * sets the virtual height of the polygon in the octal tree
     * @param h the height of the polygon in the octal tree
     */
    void setHeight(Height h);

    /**
     * tests whether the line intersects the polygon or lies within
     * @param p start point 
     * @param q the intersection point if the test line crosses the surface 
     * @param intersect the test line that is parallel to the RAY_AXIS 
     *  and passes through p crosses the polygon
     * @param inside true if the orientation vector of the polygon points 
     *  in the direction p when in q
     * @return true
     */
    virtual bool testLine(GeomPoint p, GeomPoint &q, 
        bool &intersect, bool &inside);

    /**
     * returns the idx-th corner point
     * @param idx number of the corner point
     * @return corner point no. idx
     * @pre 0 <= idx < getCount()
     */
    GeomVec operator[](int idx);

    private:
    
    /**
     * checks whether p lies on the border of the polygon
     * @param p point
     * @return p lies on the border of the polygon or not
     */
    bool isAtBorder(GeomPoint p);

    /**
     * checks whether p is a corner point of this polygon
     * @return p is a corner point of this polygon
     */
    bool isCorner(GeomPoint p);

    /**
     * returns whether q is inside the polygon
     * 
     * the rotation direction method on the projection plane is used
     * 
     * @param q point
     * @param proj axis, is needed for defining the projection plane
     * @pre isInPlane(q)
     */
    bool isIn(GeomPoint q, Axis proj);

    /**
     * number of corner points, 3 in case of triangles, 
     *                          4 in case of quadrilaterals
     */
    int m_cornerCount;

    /**
     * corner point A
     */
    GeomPoint m_pointA;

    /**
     * corner point B
     */
    GeomPoint m_pointB;

    /**
     * corner point C
     */
    GeomPoint m_pointC;

    /**
     * cornerpoint D, is the same as C for triangles
     */
    GeomPoint m_pointD;

}; // class Polygon

#endif // ! __POLYGON_H__
