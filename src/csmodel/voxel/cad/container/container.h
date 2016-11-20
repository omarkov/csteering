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
 * @file voxel/cad/container/container.h
 * interface of container for CAD-objects
 *
 * @author Mikhail Prokharau
 * @date 2004
 *
 */

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include <utils/exception.h>
#include <cad/objects/face.h>
     
/**
 * type for container objects
 */
typedef Face* Element;
     
// ##### Exceptions ################################################
   
/**
 * is thrown in case of an attempt to read or manipulate
 * a non-existent object from the CAD-model
 */
class OutsideOfModelException : public Exception {
    public:

    /**
     * exception constructor.
     */
    OutsideOfModelException() : Exception
        ("Access outside of the CadModel container") {}
        
    virtual ~OutsideOfModelException() {} 
        
    virtual ErrorType getErrorType() {
    	return OUTSIDE_OF_MODEL_ERROR;
    }    
};     

// ##### Container #################################################      
/**
 * container for CAD-objects
 */
class Container {
    public:
    
    /**
     * inserts the object into the container
     * @param object the element being inserted
     * @pre object != NULL
     */
    virtual void add(Element object)= 0;

    /**
     * number of objects in the container
     * @return Number of objects in the container
     */
    virtual unsigned count()= 0;

    /**
     * is the container empty?
     * @return true,  if there are no objects in the container\n
     *         false, otherwise
     */
    virtual bool empty()= 0;

    /**
     * is the object in the container?
     *
     * @param object object being looked for
     * @return true,  if the object is in the container\n
     *         false, otherwise
     */
    virtual bool exist(Element object)= 0;

    /**
     * sets the internal cursor of the container to the first element
     *
     * if the container is empty an OutOfModelException is thrown
     */
    virtual void first()= 0;

    /**
     * returns the object, at which the internal cursor is pointing
     * @exception OutOfModelException if hasObject() == false
     * @return current element
     */
    virtual CadObject* getObject() throw (OutsideOfModelException*)= 0;

    /**
     * returns whether the internal cursor is pointing at the
     * position inside the container
     * @return false, if the cursor is outside the container
     *                (i.e. is pointing at the position behind
     *                the last element)
     *         true,  otherwise
     */
    virtual bool hasObject()= 0;

    /**
     * sets the internal cursor to the next element
     * @exception OutOfModelException if hasObject() had already
     * returned false before next() was used
     */
    virtual void next() throw (OutsideOfModelException*)= 0;

    /**
     * prints all the elements inside the container to the console
     */
    virtual void print()= 0;

}; // class Container

#endif // ! __CONTAINER_H__
