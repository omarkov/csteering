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
 * @file voxel/cad/container/faces.h
 * surface container
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */
#ifndef __FACES_H__
#define __FACES_H__

#include <cad/container/container.h>
#include <cad/objects/face.h>

#include <list>
#include <assert.h>

/**
 * surface container
 */
class Faces : public Container {
    public:
    
    struct Iterator;

    /**
     * default constructor
     */
    Faces();

    /**
     * constructor, the object is inserted into the face container
     * @param object the surface object to be inserted
     */
    Faces(Face* object);

    /**
     * destructor
     */
    virtual ~Faces();

    /**
     * inserts the object into the container
     * @param object the object to be inserted
     * @pre object != NULL
     */
    void add(Face* object);

    /**
     * clears the container
     */
    void clear();

    /**
     * returns the number of objects currently in the container
     * @return the number of objects in the container
     */
    unsigned count();

    /**
     * is the container empty?
     * @return true,  if there are no objects in the container
     *         false, otherwise
     */
    bool empty();

    /**
     * sets the internal cursor of the container to the first element
     *
     * if the container is empty, the following call of getObject() or next() 
     * will result in OutOfModelException.
     */
    void first();

    /**
     * returns the object, at which the internal cursor is pointing
     * 
     * @exception OutOfModelException if hasObject() == false
     * @return the current element
     */
    Element getObject() throw (OutsideOfModelException*);

    /**
     * is the internal cursor pointing at an element inside the container?
     * 
     * @return \em false, if the cursor is outside of the container 
     *         \em true,  otherwise
     */
    bool hasObject();

    /**
     * sets the internal cursor to the next consecutive element
     * @exception OutOfModelException if hasObject() was false
     *  before the call of next()
     */
    void next() throw (OutsideOfModelException*);

    /**
     * prints all the elements to the console
     */
    void print();

    private:
  
    /**
     * type for surface container
     */
    typedef std::list<Face *> FaceContainer;

    /**
     * is the object inside the container?
     * 
     * @param object the object being looked for
     * @return true,  if the object is in the container
     *         false, otherwise
     */
    bool exist(Face* object);

    /**
     * is the iterator pointing at an element inside the container?
     * @param it iterator (cursor)
     * @return whether the cursor is pointing at an element inside the container
     */
    bool exist(Iterator it);

    /**
     * looks for the object inside the container
     * @param object the object being looked for
     * @return a cursor pointing at the first element in the container 
     *  that is identical to the object, if the object is not in the container,
     *  a cursor pointing outside of the container is returned
     */
    Iterator find(Face* object);

    /**
     * internal iterator
     */
    struct Iterator {  
        /**
         * iterator of the FaceContainer
         */
        FaceContainer::iterator m_faceIt;
    };

    /**
     * cursor pointing at the current object of the container
     */
    Iterator m_currentObject;

    /**
     * surface container
     */
    FaceContainer m_objs;
}; // class Faces

#endif // ! __FACES_H__
