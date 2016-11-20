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
 * file: voxel/octree/idx_holder.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <octree/idx_holder.h>

#if !defined(CLASSIC_MODE) && defined(FILL_SOLIDS)

// ##### get() #######################################################
NodeIndex IdxHolder::get() {
#if defined(MARK_BORDER) || defined(LIMITED_STACK)

    #ifdef USE_QUEUE
    
    return DataQueue<NodeIndex>::get();

    #else // !USE_QUEUE
    
    return DataStack<NodeIndex>::get();

    #endif // !USE_QUEUE

    #else // !MARK_BORDER && !LIMETED_STACK

    assert (!isEmpty());

    #ifdef USE_QUEUE
    
    NodeIndex retVal = m_pool.front();
    
    #else // !USE_QUEUE
    
    NodeIndex retVal = m_pool.top();
    
    #endif // !USE_QUEUE

    m_pool.pop();
    return retVal;

    #endif // !MARK_BORDER && !LIMITED_STACK
}

// ##### isEmpty() ###################################################
bool IdxHolder::isEmpty() {
    
    #if defined(MARK_BORDER) || defined(LIMITED_STACK)

    #ifdef USE_QUEUE
    
    return DataQueue<NodeIndex>::isEmpty();
    
    #else // !USE_QUEUE
    
    return DataStack<NodeIndex>::isEmpty();
    
    #endif // !USE_QUEUE

    #else // !MARK_BORDER && !LIMITED_STACK

    return m_pool.empty();

    #endif // !MARK_BORDER && !LIMITED_STACK
}

// ##### isFull() ####################################################
bool IdxHolder::isFull() {
    #if defined(MARK_BORDER) || defined(LIMITED_STACK)

    #ifdef USE_QUEUE
    
    return DataQueue<NodeIndex>::isFull();
    
    #else // !USE_QUEUE
    
    return DataStack<NodeIndex>::isFull();
    
    #endif // !USE_QUEUE

    #else // !MARK_BORDER && !LIMETED_STACK

    return false;

    #endif // !MARK_BORDER && !LIMITED_STACK
}

// ##### put() #######################################################
void IdxHolder::put(NodeIndex item) {
    
    #if defined(MARK_BORDER) || defined(LIMITED_STACK)

    #ifdef USE_QUEUE
    
    DataQueue<NodeIndex>::put(item);
    
    #else // !USE_QUEUE
    
    DataStack<NodeIndex>::put(item);
    
    #endif // !USE_QUEUE

    #else // !MARK_BORDER && !LIMETED_STACK

    m_pool.push(item);

    #endif // !MARK_BORDER && !LIMITED_STACK
}

#endif // !CLASSIC_MODE && FILL_SOLIDS

// EOF: voxel/octree/idx_holder.cpp
