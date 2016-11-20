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
 * @file voxel/cad/objects/point.h
 * defines the type Point
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __POINT_H__
#define __POINT_H__

#include <utils/global.h>
#include <cad/objects/cad_object.h>

#include <math.h>
#include <values.h>

/**
 * point of origin
 */
#define NULL_POINT Point(0, 0, 0)

/**
 * the maximum possible point
 */
#define MAXIMUM_POSSIBLE_POINT Point(MAXFLOAT, MAXFLOAT, MAXFLOAT)

/**
 * the minimum possible point
 */
#define MINIMUM_POSSIBLE_POINT Point(-MAXFLOAT, -MAXFLOAT, -MAXFLOAT)

/**
 * concrete class for a point
 */
class Point : public CadObject {
    public:
    
    /**
     * default constructor
     */
    Point();
    
    /**
     * constructor
     * @param x x-coordinate
     * @param y y-coordinate
     * @param z z-coordinate
     * @pre DIMENSIONS = 3
     */
    Point(Coordinate x,  //= (Coordinate)0, 
          Coordinate y,  //= (Coordinate)0, 
          Coordinate z); //= (Coordinate)0);
    
    /**
     * copy constructor
     * @param point the Point object to be copied
     * @pre DIMENSIONS = 3
     */
    Point(const Point& point);
    
    /**
     * returns the coordinate on the axis
     * @param axis the coordinate axis
     * @return coordinate on the axis
     * @pre axis < DIMENSIONS
     */
    Coordinate getCoordinate(Axis axis);

    /**
     * returns the type of the CAD object
     * @return CadObject::POINT
     */
    virtual const DataType getDataType();

    /**
     * returns the number of dimensions of a point
     * @return 0
     */
    virtual int getDimension();

    /**
     * returns the maximal point
     * @return the maximal point
     */
    virtual Point getMaxPoint();

    /**
     * returns the minimal point
     * @return the minimal point
     */
    virtual Point getMinPoint();

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
     * sets the coordinate of the axis to the value
     * @param axis coordinate axis
     * @param value new value
     * @pre axis < DIMENSIONS
     */
    void setCoordinate(Axis axis, Coordinate value);

    /**
     * sets the coordinate of the axis x to the value
     * @param value new value
     */
    void setX(Coordinate value);

    /**
     * sets the coordinate of the axis y to the value
     * @param value new value
     */
    void setY(Coordinate value);

    /**
     * sets the coordinate of the axis z to the value
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
}; // class Point

// ##### point comparison operators ##################################

/**
 * checks whether all the coordinates of the points p1 and p2 are 
 * identical
 * @param p1 the first point
 * @param p2 the second point
 * @return \f$\forall axis \in [0;\dim) :
 *               p1[axis] = p2[axis] \f$
 */
bool operator==(Point p1, Point p2);

/**
 * checks whether the point p1 is lexographically smaller than 
 * the point p2
 * @param p1 the first point
 * @param p2 the second point
 * @return \f$\exists \{ axis \in [0;\dim)~ \forall a \in [0;axis) |
 *                         p1[a] = p2[a] 
 *                           \land p1[axis] < p2[axis] \} \f$
 */
bool operator<(Point p1, Point p2);

/**
 * checks whether all the coordinates of the point p1 are smaller 
 * than those of the point p2
 * @param p1 the first point
 * @param p2 the second point
 * @return \f$\forall axis \in [0;\dim) :
 *               p1[axis] < p2[axis] \f$
 */
bool operator<<(Point p1, Point p2);

#endif // ! __POINT_H__
