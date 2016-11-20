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
 * @file voxel/cad/cad_model.h
 * defines the container for all CAD objects
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */
 
#ifndef __CAD_MODEL_H__
#define __CAD_MODEL_H__

#include <iostream>
#include <cad/container/faces.h>
#include <cad/objects/point.h>
#include <exception.h>

/**
 * container that contains all CAD objects
 *
 * the iterator is internal and can be controlled 
 * via the functions first() and next()
 */
class CadModel : public Faces {
    
    public:
  
    /**
     * default constructor
     */
    CadModel();

    /**
     * inserts the object into the CadModel if it is not already there
     * minPoint (\ref getMinPoint()) and maxPoint (\ref getMaxPoint()) are
     * automatically adjusted if needed.
     * 
     * @param object the object to be inserted
     * @param color the colour of the object
     * @pre object != NULL
     */
    void add(Element object, Color color);

    /**
     * returns the outermost rear upper right point of the area, in which 
     * all the objects are contained
     * 
     * @return current maximal point
     */
    Point getMaxPoint();

    /**
     * returns the outermost front lower left point of the area, in which 
     * all the objects are contained
     * 
     * @return current minimal point 
     */
    Point getMinPoint();
    
    /**
     * returns the outermost rear upper right point of the area, in which 
     * all the objects are contained
     * 
     * @return \f$ \sum_{i=0}^{\dim-1} \max_{p \in \mbox{CadModel container}}
     *                                   \{ p[i] \} * \vec e_i \f$ 
     */
    Point getRealMaxPoint();

    /**
     * returns the outermost front lower left point of the area, in which 
     * all the objects are contained
     * 
     * @return \f$ \sum_{i=0}^{\dim-1} \min_{p \in \mbox{CadModel container}}
     *                                   \{ p[i] \} * \vec e_i \f$ 
     */
    Point getRealMinPoint();
    
    /**
     * sets the maximal point of the CadModel, throws ModelBoundaryExc if 
     * the operation would lead to inconsistency of the CadModel
     * 
     * @param maxPoint new maximal point
     */
    void setMaxPoint(Point maxPoint);
    
    /**
     * sets the minimal point of the CadModel, throws ModelBoundaryExc if 
     * the operation would lead to inconsistency of the CadModel
     * 
     * @param minPoint new minimal point
     */
    void setMinPoint(Point minPoint);

    /**
     * returns the colour of the object, at which the internal cursor is
     * currently pointing
     * 
     * @exception OutOfModelException if hasObject() == false
     * @return colour of the current element
     */
    Color getObjColor() throw (OutsideOfModelException*);

    /**
     * prints all of the objects contained in the CadModel container to 
     * the console
     * 
     * if the CadModel container is empty, \n
     * <tt>CadModel: is empty.</tt>\n
     * is printed.
     */
    void print();  

    private:
  
    /**
     * the outermost front upper right point
     * @see getMaxPoint()
     * @see Point::max()
     */
    Point m_maxPoint;

    /**
     * the outermost rear lower left point
     * @see getMinPoint()
     * @see Point::min()
     */
    Point m_minPoint;

    /**
     * the real outermost front upper right point
     * @see getMaxPoint()
     * @see Point::max()
     */
    Point m_realMaxPoint;

    /**
     * the real outermost rear lower left point
     * @see getMinPoint()
     * @see Point::min()
     */
    Point m_realMinPoint;

    /**
     * the colour of the container objects
     */
    Color m_objColor;

}; // class CadModel

/**
 * is thrown in case of a general model error
 */
class ModelExc : public Exception {
    public:
    
    ModelExc() : Exception("General model error") {
    }
    
    ModelExc(std::string msg) : Exception(msg) {
    }
    
    virtual ~ModelExc(){}
    
    virtual ErrorType getErrorType() {
    	return MODEL_ERROR;
    }    
};

/**
 * is thrown in case of a model boundary error
 */
class ModelBoundaryExc: public ModelExc {
    public:
    
    ModelBoundaryExc() : ModelExc("Model boundary error") {
    }
    
    virtual ~ModelBoundaryExc() {};
    
    virtual ErrorType getErrorType() {
    	return MODEL_BOUNDARY_ERROR;
    }
};

#endif // ! __CAD_MODEL_H__
