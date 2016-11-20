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
 * @file voxel/geom/node_index.h
 * defines an index for the nodes that are contained in an OctStruct
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __NODE_INDEX_H__
#define __NODE_INDEX_H__

#include <global.h>

/**
 * height of a leaf node on the lowest plane
 */
#define BASE_NODE_HEIGHT 0

/**
 * index type for raster in the direction of an axis
 */
typedef int AxIndex;

/**
 * the highest value of the index type
 */
#define MAX_AX_INDEX INT_MAX

/**
 * direction type of a coordinate axis
 */
typedef int AxDirection;

/**
 * direction backward
 */
const AxDirection BACKWARD = -1;

/**
 * direction forward
 */
const AxDirection FORWARD = +1;

//class NodeVec;

/**
 * class defining the index for an octal tree
 *
 * an integer index is defined for every dimension,
 * the index range is derived from the height
 * \f$ [0; 2^{\mbox{height}}-1] \f$.
 *
 * NodeIndex is used by OctStruct, height is the height of the point 
 * in the octal tree structure, the indices in all directions of space
 * uniquely define the point in the octal tree segment, for an indexed 
 * point of the octal tree structure the difference to its neighbour 
 * equals 1
 */
class NodeIndex {
    
    public:
  
    /**
     * constructor
     * @param x index x
     * @param y index y
     * @param z index z
     * @param h height of the point in the octal tree strcuture
     * @pre DIMENSIONS = 3
     */
    NodeIndex(AxIndex x, AxIndex y, AxIndex z, Height h = BASE_NODE_HEIGHT);

    /**
     * returns the index in the direction of the axis
     * @param axis index axis
     * @return index on the axis
     * @pre axis < DIMENSIONS
     */
    AxIndex getCoordinate(Axis axis);

    /**
     * returns the height of the point in the octal tree structure
     * @return the height of the point
     */
    Height getHeight();

    /**
     * returns the index in the direction x
     * @return x index x
     */
    AxIndex getX();

    /**
     * returns the index in the direction y
     * @return index y
     */
    AxIndex getY();

    /**
     * returns the index in the direction z
     * @return index z
     */
    AxIndex getZ();

    /**
     * prints the point indices and their height to the console
     */
    virtual void print();

    /**
     * sets the indices of the point
     * @param x new index x
     * @param y new index y
     * @param z new index z
     * @pre DIMENSIONS = 3
     */
    void set(AxIndex x, AxIndex y, AxIndex z);

    /**
     * sets the index in the direction of the axis to the value
     * @param axis index axis
     * @param value new value
     * @pre axis < DIMENSIONS
     */
    void setCoordinate(Axis axis, AxIndex value);

    /**
     * sets the height of the point in the octal tree structure
     * @param h height
     */
    void setHeight(Height h);

    /**
     * sets the index in the direction x
     * @param x index x
     */
    void setX(AxIndex x);

    /**
     * sets the index in the direction y
     * @param y index y
     */
    void setY(AxIndex y);

    /**
     * sets the index in the direction z
     * @param z index z
     */
    void setZ(AxIndex z);

    /**
     * equivalent to getCoordinate(axis).
     * @param axis number of the axis
     * @return index on the axis
     */
    AxIndex operator[](Axis axis);

    private:
  
    /**
     * indices of the point
     */
    AxIndex m_indices[DIMENSIONS];

    /**
     * height of the point
     */
    Height m_height;
};

/**
 * checks whether p1 and p2 reference the same node in the octal tree
 * @param p1 the frist node vector
 * @param p2 the second node vector
 * @return \f$ \forall i \in [0;\dim) : p_1[i]
 *                                    = p_2[i]
 *         \f$
 * @pre p1.getHeight() = p2.getHeight()
 */
bool operator==(NodeIndex p1, NodeIndex p2);

/**
 * checks whether p1 is lexographically followed by p2
 * @param p1 the first node vector
 * @param p2 the second node vector
 * @return \f$\exists i \in [0; \dim) :
 *            p_1[i] < p_2[i] ~ 
              \wedge ~ \forall k \in [0; i)  : p_1[k] = p_2[k] \f$
 * @pre p1.getHeight() = p2.getHeight()
 */
bool operator<(NodeIndex p1, NodeIndex p2);

#endif // ! __NODE_INDEX_H__
