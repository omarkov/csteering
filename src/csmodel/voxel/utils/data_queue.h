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
 * @file voxel/utils/data_queue.h
 * data queue
 *
 * template for a queue
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __DATA_QUEUE_H__
#define __DATA_QUEUE_H__

#include <data_holder.h>

/**
 * queue
 */
template <class ItemType> class DataQueue : protected DataHolder<ItemType> {
    public:
  
    /**
     * constructor
     * @param size maximal size (no. of items) of the data queue to be created
     */
    DataQueue(unsigned long size) throw (NotEnoughMemoryException) 
        : DataHolder<ItemType>(size), m_first(0) {
    }

    /**
     * returns the current item and deletes it from the data queue
     * @return curent element
     * @pre !isEmpty()
     */
    inline ItemType get() {
        assert (!isEmpty());
    
        ItemType retVal = m_pool[m_first];
        m_first = (m_first + 1) % getSize();
        return retVal;
    }
  
    /**
     * checks whether the data queue is empty
     * @return the data queue is empty or not
     */
    inline bool isEmpty() {
        return m_first == m_last;
    }

    /**
     * checks whether the data queue is full
     * @return the data queue is full or not
     */
    inline bool isFull() {
        return ((m_last + 1) % getSize()) == m_first;
    }

    private:  
  
    /**
     * the first element of the queue
     */
    unsigned long m_first;
 
};

#endif // ! __DATA_QUEUE_H__
