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
 * @file voxel/geom/node_vec.h
 * defines vectors between indexed octal tree nodes
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __NODE_VEC_H__
#define __NODE_VEC_H__

#include <node_index.h>
#include <assert.h>

/**
 * 0-Vektor.
 */
#define NULL_NODE_VEC NodeIndex(0, 0, 0)

/**
 * direction type for the whole vector
 */
typedef NodeIndex Direction;

/**
 * class for vectors between octal tree nodes
 */
class NodeVec : public NodeIndex {
    public:
  
    /**
     * default constructor
     * creates NodeVec based on the NodeIndex nodeIdx
     * @param nodeIdx index in the octal tree
     */
    NodeVec(NodeIndex nodeIdx);

    /**
     * constructor
     * creates the NodeVec that represents the vector
     * \f$\over{\mbox{pStart}\ \mbox{pEnd}}\f$ 
     * @param pStart source node
     * @param pEnd target node
     * @pre pStart.getHeight() = pEnde.getHeight()
     */
    NodeVec(NodeVec pStart, NodeVec pEnd);

    /**
     * returns the direction of the node vector
     * 
     * it is decided for each direction in space, whether the source node is
     * above (+1), below (-1) or at the same height as the target node, 
     * the direction is derived from the sign of the respective axis indices
     * @return direction of the node vector
     */
    Direction getDirection();
};

/**
 * returns the resulting difference vector
 * @param p1 the first node vector
 * @param p2 the second node vector
 * @return p1-p2
 * @pre p1.getHeight() = p2.getHeight()
 */
NodeVec operator-(NodeVec p1, NodeVec p2);

#endif // ! __NODE_VEC_H__
