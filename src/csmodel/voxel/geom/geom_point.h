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
 * @file voxel/geom/geom_point.h
 * defines the node index in (R, R, R)
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __GEOM_POINT_H__
#define __GEOM_POINT_H__

#include <global.h>
#include <node_index.h>

/**
 * offset to the cell centre
 */
static const Coordinate OFS = 0.5;

/**
 * defines the node index in (R, R, R)
 */
class GeomPoint {
    public:
  
    /**
     * default constructor
     * @param x x-coordinate
     * @param y y-coordinate
     * @param z z-coordinate
     * @param h height of the corresponding node index in the octal 
     *  tree structure
     * @pre DIMENSIONS = 3
     */
    GeomPoint(Coordinate x = 0, Coordinate y = 0, Coordinate z = 0,
        Height h = BASE_NODE_HEIGHT);

    /**
     * constructor
     * @param p NodeIndex
     */
    GeomPoint(NodeIndex p);

    /**
     * returns the direction of rotation of the projection of the point 
     * in respect to the points p1 and p2
     * @param p1 point p1
     * @param p2 point p2
     * @param proj axis perpendicular to the projection plane
     * @return \b 0, if the point does not form an actual triangle 
     *                 (only a line or a point) 
     *         \b 1, the points lie anticlockwise
     *         \b -1, the points lie clockwise
     */
    int ccw(GeomPoint p1, GeomPoint p2, Axis proj);

    /**
     * returns the Axindex corresponding to the coordinate getCoordinate(axis)
     * it is always true for any regular object of the type NodeIndex: 
     * p[axis] == GeomPoint(p).getAxIndex(axis)
     * @return AxIndex of the coordinate getCoordinate(axis)
     */
    AxIndex getAxIndex(Axis axis);

    /**
     * returns the coordinate of the axis
     * @param axis coordinate axis
     * @return coordinate on the axis
     * @pre axis < DIMENSIONS
     */
    Coordinate getCoordinate(Axis axis);

    /**
     * returns a copy of the point
     * @return the copy
     */
    GeomPoint getDup();

    /**
     * returns the height of the corresponding node index of the octal 
     * tree structure
     * @return height
     */
    Height getHeight();

    /**
     * returns the corresponding node index, it is true for any regular 
     * object p of the type NodeIndex:
     * p == GeomPoint(p).getNodeIndex()
     * @return node index
     */
    NodeIndex getNodeIndex();

    /**
     * returns the x-coordinate
     * @return x-coordinate
     */
    Coordinate getX();

    /**
     * returns the y-coordinate
     * @return y-coordinate
     */
    Coordinate getY();

    /**
     * returns the z-coordinate
     * @return z-coordinate
     */
    Coordinate getZ();

    /**
     * prints the coordinates of the point to the console
     */
    virtual void print();

    /**
     * sets the coordinates of the point
     * @param x new value of the x-coordinate
     * @param y new value of the y-coordinate
     * @param z new value of the z-coordinate
     * @pre DIMENSIONS = 3
     */
    void set(Coordinate x, Coordinate y, Coordinate z);

    /**
     * sets the coordinate of the coordinate axis to the new value
     * @param axis coordinate axis
     * @param value new value
     * @pre axis < DIMENSIONS
     */
    void setCoordinate(Axis axis, Coordinate value);

    /**
     * sets the height of the corresponding node index in the octal 
     * tree structure
     * @param h height
     */
    void setHeight(Height h);

    /**
     * sets the coordinate of the x axis to the new value
     * @param value new value
     */
    void setX(Coordinate value);

    /**
     * sets the coordinate of the y axis to the new value
     * @param value new value
     */
    void setY(Coordinate value);

    /**
     * sets the coordinate of the z axis to the new value
     * @param value new value
     */
    void setZ(Coordinate value);

    /**
     * equivalent to getCoordinate(axis).
     * @return coordinate on the axis
     */
    Coordinate operator[](Axis axis);

    private:
    
    /**
     * coordinates of the point
     */
    Coordinate m_coordinates[DIMENSIONS];

    /**
     * height of the corresponding node index
     */
    Height m_indexHeight; 
}; // class GeomPoint

// ##### corresponding comparison operators ##########################

/**
 * checks whether all the coordinates of the points p1 and p2 are 
 * pairwise identical
 * @param p1 the first point
 * @param p2 the second point
 * @return \f$\forall axis \in [0;\dim) :
 *               p1[axis] = p2[axis] \f$
 * @pre p1.getHeight() = p2.getHeight()
 */
bool operator==(GeomPoint p1, GeomPoint p2);

#endif // ! __GEOM_POINT_H__
