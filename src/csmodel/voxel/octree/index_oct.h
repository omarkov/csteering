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
 * @file voxel/octree/index_oct.h
 * indexed octal tree
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __INDEX_OCT_H__
#define __INDEX_OCT_H__

#include <octree/oct_struct.h>
#include <node_index.h>
#include <polygon.h>
#include <point.h>
#include <exception.h>

/**
 * number of subpartitions per axis
 */
const Axis AXIS_PARTS = 2;

/**
 * indexed octal tree
 *
 * makes octal tree processing operations available, the nodes of the octal 
 * tree structure are referenced via NodeIndex
 */
class IndexOct : public OctStruct {
    public:
    
    /**
     * constructor
     * @param maxTreeHeight maximal tree height
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     */
    IndexOct(Height maxTreeHeight) throw(NotEnoughMemoryException*);

    /**
     * constructor
     * @param maxTreeHeight maximal tree height
     * @param minPoint minimal point
     * @param maxPoint maximal point
     * @param minGeomPoint minimal geometric point
     * @param maxGeomPoint maximal geometric point
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     */
    IndexOct(Height maxTreeHeight, Point minPoint, Point maxPoint,
        GeomPoint minGeomPoint, GeomPoint maxGeomPoint)
        throw(NotEnoughMemoryException*);

    /**
     * inserts the new node with the index p and the colour color into 
     * the octal tree structure
     * @param p node index
     * @param color node colour
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     * @pre 0 <= p.getHeight() <= getMaxTreeHeight()
     */
    void add(NodeIndex p, Color color) throw (NotEnoughMemoryException*);

    /**
     * inserts the surface s with the colour color into the octal tree
     * @param s surface
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure 
     * @pre s != NULL
     * @pre s.getHeight() = BASE_NODE_HEIGHT
     */
    void add(Surface* s, Color color) throw (NotEnoughMemoryException*);

    /**
     * checks whether the node with the NodeIndex p is in the octal tree
     * @param p node index
     * @return p references the root node or there is a father node of p and 
     * therefore the father node of p is not a leaf
     */
    bool exist(NodeIndex p);

    /**
     * returns the index of the element no. i
     * 
     * @param parent parent index
     * @param i no. of the subpartition
     * @return child node index no. i
     * @pre parent.getHeight() != BASE_NODE_HEIGHT and isIn(parent)
     */
    NodeIndex getChild(NodeIndex parent, PartType i);

    /**
     * returns the node colour, p can't reference an inner node, if p does not 
     * exist, the colour of its virtual father node is returned
     * 
     * @param p node index
     * @pre isLeaf(p') with p' being the deepest existing father node
     */
    Color getColor(NodeIndex p);

    /**
     * if p is the index of an existing node, p is returned otherwise the 
     * deepest existing father node
     * @param p node index
     * @return the deepest existing father node (p is included in the set)
     * @post getExistNode(p).getHeight() >= p.getHeight()
     */
    NodeIndex getExistNode(NodeIndex p);

    /**
     * returns the maximal tree height
     * @return maximal tree height
     */
    Height getMaxTreeHeight();

    /**
     * returns the minimal point
     * @return minimal point
     */
    Point getMinPoint();
  
    /**
     * returns the maximal point
     * @return maximal point
     */
    Point getMaxPoint();
  
    /**
     * returns the minimal geometric point
     * @return minimal geometric point
     */
    GeomPoint getMinGeomPoint();
    
    /**
     * returns the maximal geometric point
     * @return maximal geometric point
     */
    GeomPoint getMaxGeomPoint();

    /**
     * returns the offset resulting from the subpartition number and the 
     * axis number
     * 
     * @param partNumber subpartition number
     * @param axis axis
     * @return \f$ ( \mbox{partNumber} \triangleleft \mbox{axis} )
     *               \bmod \mbox{AXIS\_PARTS} \f$
     */
    static AxIndex getPartOfs(PartType partNumber, Axis axis);

    /**
     * if p is a leaf node, p must exist
     * 
     * @param p node index
     * @return p is a leaf node or not
     * @pre exist(p)
     */
    bool isLeaf(NodeIndex p);

    /**
     * checks whether p can be inside the octal tree if the tree is fully
     * occupied (all leaves are at the same height BASE_NODE_HEIGHT)
     * @param p node index
     * @return \f$ \forall_{i} \in [0;\dim) :
     *                0 <= p[i] < 1 \triangleright \mbox{getMaxTreeHeight} \f$
     */
    bool isIn(NodeIndex p);

    /**
     * sets the minimal point
     * @param minPoint minimal point
     */
    void setMinPoint(Point minPoint);
  
    /**
     * sets the maximal point
     * @param maxPoint maximal point
     */
    void setMaxPoint(Point maxPoint);
    
    /**
     * sets the minimal geometric point
     * @param minGeomPoint minimal geometric point
     */
    void setMinGeomPoint(GeomPoint minGeomPoint);
  
    /**
     * sets the maximal geometric point
     * @param maxGeomPoint maximal geometric point
     */
    void setMaxGeomPoint(GeomPoint maxGeomPoint);

    /**
     * sets the node colour, p can't reference an inner node, if p does not 
     * exist, the colour of its virtual father node is set
     * @param p node index
     * @param color new node colour
     * @pre isLeaf(p')
     */
    void setColor(NodeIndex p, Color color);
  
    /**
     * sets minimal and maximal points as well as minimal and maximal 
     * geometric points
     */
    void setPoints(Point minPoint, Point maxPoint, GeomPoint minGeomPoint,
        GeomPoint maxGeomPoint);

    protected:
    
    /**
     * if p is an index of an existing node, p.getHeight() is returned, 
     * otherwise the height of the deepest existing father node
     * @param p node index
     * @param subtree pointer to the existing node
     * @return branch depth
     * @post getExistNode(p, node) >= p.getHeight()
     * @post octree != NULL
     */
    Height getExistNode(NodeIndex p, _octree &subtree);
    
    /**
     * checks whether the node with the index p is below the height h
     * @param p node index
     * @param h comparison height
     * @return p.getHeight() >= h
     * \note is h the minimal leaf height of the branch where p is, 
     * \em isIn(p, h) is returned
     */
    bool isIn(NodeIndex p, Height h);

    #ifdef MARK_BORDER
    
    /**
     * sets p as the border node with the normal colour
     * @param p leaf node index
     * @param color normal colour
     * @pre isLeaf(p)
     * @pre color >= NO_OBJECT
     */
    void setBorderColor(NodeIndex p, Color color);

    #endif

    private:
  
    /**
     * inserts the nodes that belong to the surface s into the subtree,
     * myIdx corresponds to the index of the root node of the subtree,
     * myIdx is needed for checking to what extent the subtree is relevant 
     * for the insertion of the surface
     * @param parent octal subtree
     * @param myIdx index of the root node of the octal subtree
     * @param s surface
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structures
     * @pre isIn(myIdx) 
     * @pre s != NULL
     */
    void add(_octree parent, NodeIndex myIdx, Surface* s, Color color)
        throw (NotEnoughMemoryException*);

    /**
     * returns the subpartition of the axis for the node on the branch at the 
     * height h
     * 
     * @param idx axis index of the node
     * @param h height of the position of the branch
     * @return subpartition for the axis at the position h
     */
    PartType getPart(AxIndex idx, Height h);

    /**
     * returns the subpartition for the node on the branch at the 
     * height h
     * @param p node index
     * @param h height of the position of the branch
     * @return subpartition at the position h
     */
    PartType getPart(NodeIndex p, Height h);

    /**
     * height of the root node, is identical to the maximal height of the tree
     */
    Height m_rootHeight;
  
    /**
     * the minimal point
     */
    Point m_minPoint;
  
    /**
     * the maximal point
     */
    Point m_maxPoint;
  
    /**
     * the minimal geometric point
     */
    GeomPoint m_minGeomPoint;
  
    /**
     * the maximal geometric point
     */
    GeomPoint m_maxGeomPoint;
};

#endif // ! __INDEX_OCT_H__
