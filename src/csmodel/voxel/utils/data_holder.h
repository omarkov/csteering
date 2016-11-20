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
 * @file voxel/utils/data_holder.h
 * virtual data memory
 *
 * virtual template for conventional containers (stack, queue)
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __DATA_HOLDER_H__
#define __DATA_HOLDER_H__

#include <exception.h>

#include <assert.h>
#include <stdlib.h>

/**
 * virtual data memory
 */
template <class ItemType> class DataHolder {
    public:
    
    /**
     * constructor
     * @param size maximal size of the memory to be created
     */
    DataHolder(unsigned long size) throw (NotEnoughMemoryException) 
        : m_last(0), m_size(size) {
        
        pool = (ItemType*)malloc(size * sizeof(ItemType));
    
        if (pool == NULL) {
            throw new NotEnoughMemoryException();
        }
    }

    /**
     * destructor
     */
    virtual ~DataHolder() {
        free(pool);
    }

    /**
     * returns the current item and deletes it from the data memory
     * @return current element 
     * @pre !isEmpty()
     */
    virtual ItemType get() = 0;

    /**
     * checks whether the data memory is empty
     * @return the data memory is empty or not
     */
    virtual bool isEmpty() = 0;

    /**
     * checks whether the data memory is full
     * @return the data memory is full or not
     */
    virtual bool isFull() = 0;

    /**
     * puts the element item into the data memory
     * @param item element
     * @pre !isFull()
     */
    inline void put(ItemType item) {
        assert (!isFull());
    
        m_pool[m_last] = item;
        m_last = (m_last + 1) % getSize();
    }

    protected:  
  
    /**
     * returns the maximal size of the data memory
     * @return size of the data memory
     */
    inline unsigned long getSize() {
        return m_size;
    }
  
    /**
     * the last element of the data memory
     */
    unsigned long m_last;
 
    /**
     * item container
     */
    ItemType* m_pool;

    private: 
  
    /**
     * size of the data memory
     */
    unsigned long m_size;
};

#endif // ! __DATA_HOLDER_H__
