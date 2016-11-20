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
 * @file voxel/octree/oct_struct.h
 * octal tree structure
 *
 * the lowest level, 
 * defines the low level octal tree structure operations
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __OCT_STRUCT_H__
#define __OCT_STRUCT_H__

#include <global.h>
#include <exception.h>

/**
 * type for partitions
 */
typedef int PartType;

/**
 * number of the octagon partitions, corresponds to the number of children 
 * of an inner node in the octal tree
 */
const PartType OCT_PARTS = 1 << DIMENSIONS;

/**
 * node attribute type, defines the colour of the object for leaves
 */
typedef Color NodeStatus;

/**
 * colour of a leaf that is outside all objects and therefore belongs to 
 * none of them
 */
const NodeStatus NO_OBJECT = 0;

/**
 * colour of a leaf, for which it is not known, to what object it belongs
 */
const NodeStatus UNDEF_OBJ = (-MAX_COLOR - 2);

/**
 * octal tree structure
 *
 * the lowest level
 * defines the low level octal tree structure operations
 */
class OctStruct {
    public:
    
    struct Node;
  
    /**
     * octal tree structure type 
     */
    typedef Node* _octree;

    /**
     * deletes the octal tree structure, frees the memory allocated for the 
     * octal tree and defines the root node as a leaf belonging to no object
     */
    void clear();

    /**
     * checks whether the octal tree structure is empty
     * @return \em true,  if after flush() the root node is a leaf that 
     * belongs to no tree \n
     *         \em false, otherwise
     */
    bool empty();

    /**
     * guarantees that the octal tree structure is minimized, if a node contains 
     * only leaves of the same colour, the leaves are deleted, the node becomes 
     * a leaf, its colour is set to the colour of its leaves
     */
    void flush();

    /**
     * returns the highest colour value that the octal tree leaves possess
     * @return the highest colour value that the octal tree leaves possess
     */
    Color getMaxColor();

    /**
     * returns the size of a node in memory in bytes 
     * @return the size in bytes
     */
    size_t getNodeSize();

    /**
     * returns the octal tree structure
     * @return address of the root node of the octal tree structure
     */
    _octree& getTree();

    /**
     * prints the octal tree to the console
     */
    void print();

    /**
     * returns the statistics about the model
     * @param h tree height
     * @param sumNodes number of all octal tree nodes
     * @param leaves number of all leaves
     * @param borderNodes number of all border nodes
     * @param innerNodes number of all inner nodes
     * @param normCells number of all norm cells of the lowest plane
     */
    void stat(Height h, unsigned &sumNodes, unsigned &leaves, 
        unsigned &innerNodes, unsigned &borderNodes, unsigned &normCells);

    protected:
    
    /**
     * type for nodes in the octal tree
     */
    struct Node {

        /**
         * subpartitioning child nodes for inner nodes of the octal tree,
         * NULL for leaves
         */
        struct Node *parts;

        /**
         * node attribute
         * - for leaves: leaf colour (NO_OBJECT if the leaf belongs to no tree)
         * - for inner nodes: undefined
         */
        NodeStatus flag;
    };

    /**
     * default constructor,
     * creates a new empty octal tree
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     */
    OctStruct() throw (NotEnoughMemoryException*);

    /**
     * creates a new octal tree structure with OCT_PARTS, the structure
     * belongs to no tree
     * @return new octal tree structure
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     * @post \c return != NULL
     */
    _octree createLeaves() throw (NotEnoughMemoryException*);

    /**
     * creates a leaf node that belongs to no object
     * 
     * @return new leaf node
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     * @post \c return != NULL
     */
    _octree createOneLeaf() throw (NotEnoughMemoryException*);

    /**
     * checks whether the tree exists
     * @return tree != NULL
     */
    bool exist(_octree tree);

    #ifdef MARK_BORDER
  
    /**
     * returns the correct colour of a border node
     * @param node border node
     * @return normal colour of the node
     * @pre isBorderNode(node)
     */
    Color getBorderColor(Node node);
    
    #endif

    /**
     * returns the pointer to the i-th direct subpartition
     * 
     * @param parent tree father node
     * @param i number of the subpartition
     * @return pointer to the i-th child node
     * @pre exist(parent)
     * @pre !isLeaf(*parent)
     */
    _octree getChild(_octree parent, PartType i);

    /**
     * returns the colour of a leaf node
     * @param node leaf node
     * @return colour of the leaf node
     * @pre isLeaf(node)
     */
    Color getColor(Node node);

    #ifdef MARK_BORDER
  
    /**
     * checks whether the node is marked with the border colour
     * @param node leaf node
     * @return the node is marked with the border colour or not
     */
    bool isBorderNode(Node node);

    #endif

    /**
     * checks whether the node is a leaf
     * @return true,  if the node is a leaf\n
     *         false, if the node is an inner node
     */
    bool isLeaf(Node node);

    /**
     * checks whether the node belongs to no object
     * @pre isLeaf(node)
     * return getColor(node) == NO_OBJECT
     */
    bool isNoObject(Node node);

    /**
     * checks whether it is undefined if the node belongs to an object
     * @pre isLeaf(node)
     * return getColor(node) == UNDEF_OBJ
     */
    bool isUndefObj(Node node);

    /**
     * deletes the octal tree substructure and frees the memory,
     * does nothing if subtree == NULL.
     * @param subtree octal tree substructure
     */
    void remove(_octree &subtree);

    #ifdef MARK_BORDER
    
    /**
     * sets the node to a border node with the normal colour color
     * @param node leaf node
     * @param color normal colour
     * @pre isLeaf(node)
     * @pre color >= NO_OBJECT
     */
    void setBorderColor(Node &node, Color color);
    
    #endif

    /**
     * sets the colour of the leaf
     * @param node leaf node
     * @param color leaf colour
     * @pre isLeaf(node);
     * @pre color >= NO_OBJECT
     */
    void setColor(Node &node, Color color);

    /**
     * substitutes the octal tree structure with the tree
     * to free the memory that is occupied by the old octal tree structure,
     * clear() must be explicitly called
     * @param tree new octal tree structure
     * @exception NotEnoughMemoryException not enough memory to allocate 
     *  the new octal tree structure
     */
    void setTree(_octree &tree) throw (NotEnoughMemoryException*);

    /**
     * number of the inserted border nodes
     */
    unsigned countBorderNodes;

    private:
    
    /**
     * checks whether the direct subpartitions can be bound together
     * during the test, only the direct child nodes are taken into consideration
     * @param node node
     * @param color colour of all the subpartitions if they all are of the same
     *  colour, undefined, if the direct subpartitions can't be bound
     */
    bool canCompact(Node node, Color &color);

    /**
     * minimizes the whole tree that is below the node, the subpartitions 
     * of the same colour are bound together
     * @param node node
     */
    void compact(Node &node);

    /**
     * compacts the node to a leaf of the colour color
     * @param node node
     * @param color colour
     */
    void compact(Node &node, Color color);

    /**
     * prints the octal tree that is defined by the node to the screen
     * @param node node
     */
    void printTree(Node node);

    /**
     * defines the node as a leaf node
     * @param node node
     * \note for subpartitions the allocated memory is not freed
     */
    void setLeaf(Node &node);

    /**
     * traverses the octal tree in order to gather the statistics for the model
     * @param tree octal (sub)tree
     * @param h (sub)tree height
     * @param sumNodes number of all octal tree nodes
     * @param leaves number of leaves
     * @param innerNodes number of inner nodes
     * @param normCells number of norm cells of the geometry (the lowest plane)
     */
    void stat(_octree tree, Height h, unsigned &sumNodes, unsigned &leaves, 
        unsigned &innerNodes, unsigned &normCells);

    /**
     * address of the root node of the octal tree structure
     * @invariant root != NULL
     */
    _octree m_root;
};

#endif // ! __OCT_STRUCT_H__
