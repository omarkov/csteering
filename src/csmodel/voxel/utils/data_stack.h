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
 * @file voxel/utils/data_stack.h
 * stack
 *
 * template for a stack 
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __DATA_STACK_H__
#define __DATA_STACK_H__

#include <data_holder.h>

/**
 * data stack
 */
template <class ItemType> class DataStack : protected DataHolder<ItemType> {
    public:
    
    /**
     * constructor
     * @param size maximal size (no. of items) in the stack to be created
     */
    DataStack(unsigned long size) throw (NotEnoughMemoryException) 
        : DataHolder<ItemType>(size) {
    }

    /**
     * returns the current item and deletes it from the data stack
     * @return the current element
     * @pre !isEmpty()
     */
    inline ItemType get() {
        assert (!isEmpty());
    
        m_last--;
        return m_pool[m_last];
    }
  
    /**
     * check whether the data stack is empty 
     * @return the data stack is empty or not
     */
    inline bool isEmpty() {
        return m_last == 0;
    }

    /**
     * checks whether the data stack is full
     * @return the data stack is full or not
     */
    inline bool isFull() {
        return m_last == (getSize() - 1);
    }

};

#endif // ! __DATA_STACK_H__
