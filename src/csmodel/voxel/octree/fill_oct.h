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
 * @file voxel/octree/fill_oct.h
 * octal tree that can be filled
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __FILL_OCT_H__
#define __FILL_OCT_H__

#include <global.h>

#if !defined(CLASSIC_MODE) && defined(FILL_SOLIDS)

#include <octree/index_oct.h>
#include <generator/oct_gen.h>
#include <octree/idx_holder.h>

/**
 * octal tree that can be filled
 *
 * offers operations for filling octal trees 
 */
class FillOct : public IndexOct {
    public:
  
    /**
     * constructor
     * @param idxOct indexed octal tree
     */
    FillOct(IndexOct idxOct);

    /**
     * fills the object inside the octal tree with its colour 
     * and the nodes outside the object with NO_OBJECT,
     * all undefined nodes are set to their correct colour,
     * the generator is needed for the localization of nodes 
     * (inside the body or outside)
     * @param generator octal tree generator
     * @pre generator != NULL
     */
    void fill(OctGen* generator);

    private: 
  
    /**
     * if idx is an UNDEF_OBJ, its colour is set to m_fillColor
     * @param idx node index
     */
    void add(NodeIndex idx);

    /**
     * starting from the points in m_fillPoints the surrounding area is filled 
     * with m_fillColor
     */
    void fill();
 
    /**
     * fills the part of the passed subtree with the colour that is found 
     * on the side partOfs of the axis, in case of leaf nodes, they are 
     * filled, filling means that for nodes satisfying isUndefObj() the 
     * colour is set to color, in this case the neighbours of the nodes 
     * are filled as well by being included in m_fillPoints
     * @param subtree pointo to the octal tree node myIdx
     * @param myIdx node index
     * @param axis axis used to reach the node as a neighbour
     * @param partOfs 0, if front and 1 if rear part of the subtree is to 
     *  be filled
     */
    void fillParts(_octree subtree, NodeIndex myIdx, Axis axis, 
        AxIndex partOfs);

    /**
     * fills the part of the passed myIdx with the colour that is found 
     * on the side partOfs of the axis, filling means that for nodes satisfying 
     * isUndefObj() the colour is set to color, in this case the neighbours 
     * of the nodes are filled as well by being included in m_fillPoints
     * @param myIdx node index
     *  axis used to reach the node as a neighbour
     * @param partOfs 0, if front and 1 if rear part of the subtree is to 
     *  be filled
     */
    void fillParts(NodeIndex myIdx, Axis axis, AxIndex partOfs);

    /**
     * fills the part of the passed myIdx with the colour that is found 
     * on the side partOfs of the axis, filling means that for nodes satisfying 
     * isUndefObj() the colour is set to color, in this case the neighbours 
     * of the nodes are filled as well by being included in m_fillPoints
     * @param subtree subtree
     * @param myIdx index of the father node of the subtree
     * @param generator octal tree generator
     * @pre generator != NULL
     */
    void fillTree(_octree subtree, NodeIndex myIdx, OctGen* generator);

    #ifdef SAFE_FILL
    
    /**
     * returns the colour of a node that is to be used for filling 
     * @param safefill (out) returns whether the filling colour is safe (that 
     * is whether it is the same or can be regarded safe with respect to the 
     * neighbouring colour)
     * @return filling colour
     */
    Color getColor(OctGen* generator, NodeIndex p, bool &safeFill);
    
    #endif

    /**
     * filling colour
     */
    Color m_fillColor;

    /**
     * memory for filling points
     */
    IdxHolder m_fillPoints;

    #ifdef LIMITED_STACK
    
    /**
     * states whether the filling is to be done further or be stopped because
     * of the maximal item number having been reached
     */
    bool m_fillFurther;
  
    /**
     * filling point for the further use of the filling process
     * m_fillFurther = true.
     */
    NodeIndex m_keptPoint;
    
    #endif // LIMITED_STACK
};

#endif // !CLASSIC_MODE && FILL_SOLIDS

#endif // ! __FILL_OCT_H__
