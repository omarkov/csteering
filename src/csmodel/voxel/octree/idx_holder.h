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
 * @file voxel/octree/idx_holder.h
 * intermediary memory for NodeIndex
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __IDX_HOLDER_H__
#define __IDX_HOLDER_H__

#include <global.h>

#if !defined(CLASSIC_MODE) && defined(FILL_SOLIDS)

#include <node_index.h>

#if defined(LIMITED_STACK) || defined(MARK_BORDER) 

#ifdef USE_QUEUE 

#include <utils/data_queue.h>

#else // !USE_QUEUE

#include <utils/data_stack.h>

#endif // !USE_QUEUE

#else // !LIMITED_STACK && !MARK_BORDER

#include <utils/data_holder.h>

#ifdef USE_QUEUE

#include <queue>

#else // !USE_QUEUE

#include <stack>

#endif // !USE_QUEUE

#endif // !LIMITED_STACK && !MARK_BORDER

/**
 * intermediary memeory for Nodeindex
 *
 * stack or queue for NodeIndex, is used by FillOct, queue if USE_QUEUE is on,
 * otherwise stack, if LIMITED_STACK or MARK_BORDER are on, IdxHolder is 
 * limited to MAX_RECURSION_DEPTH, if MAX_RECURSION_DEPTH <= 0, the maximal 
 * size of IdxHolder is adaptively defined based on the maximal tree height,
 * otherwise the parameter is not taken into consideration
 */
#if defined(MARK_BORDER) || defined(LIMITED_STACK)

#if MAX_RECURSION_DEPTH <= 0 

/**
 * maximal number of elements that can be stored in DataHolder (DataStack or 
 * DataQueue)
 */
#define HOLDER_SIZE (DIMENSIONS << treeHeight)

#else // MAX_RECURSION_DEPTH > 0

/**
 * maximal number of elements that can be stored in DataHolder (DataStack or 
 * DataQueue)
 */
#define HOLDER_SIZE MAX_RECURSION_DEPTH

#endif // MAX_RECURSION_DEPTH > 0

#ifdef USE_QUEUE

class IdxHolder : public DataQueue<NodeIndex> {

#else // !USE_QUEUE

class IdxHolder : public DataStack<NodeIndex> {

#endif // !USE_QUEUE

#else // !MARK_BORDER && !LIMITED_STACK

class IdxHolder {

#endif // !MARK_BORDER && !LIMITED_STACK

    public:
  
    /**
     * constrcutor
     * @param treeHeight maaximal tree height
     */
    inline IdxHolder(Height treeHeight) 

    #if defined(LIMITED_STACK) || defined(MARK_BORDER)

    #ifdef USE_QUEUE

    : DataQueue<NodeIndex>(HOLDER_SIZE) {

    #else // !USE_QUEUE

    : DataStack<NodeIndex>(HOLDER_SIZE) {

    #endif // !USE_QUEUE

    #else // !LIMITED_STACK && !MARK_BORDER
    
    {

    #endif // !LIMITED_STACK && !MARK_BORDER

    }

    /**
     * returns the current item and deletes it from the data memeory
     * @return curent element
     * @pre !isEmpty()
     */
    NodeIndex get();

    /**
     * checks whether the data memory is empty
     * @return the data memeory is empty or not
     */
    bool isEmpty(); 

    /**
     * checks whether the data memory is full
     * @return false 
     */
    bool isFull(); 

    /**
     * put the element into the data memory
     * @param item element
     */
    void put(NodeIndex item); 

    #if !defined(MARK_BORDER) && !defined(LIMITED_STACK)
    
    private:
  
    /**
     * data memeory
     */
    #ifdef USE_QUEUE
    
    std::queue<NodeIndex> m_pool;
    
    #else // !USE_QUEUE  
    
    stack<NodeIndex> m_pool;
    
    #endif // !USE_QUEUE

    #endif // !MARK_BORDER && !LIMITED_STACK

};

#endif // !CLASSIC_MODE && FILL_SOLIDS

#endif // ! __IDX_HOLDER_H__
