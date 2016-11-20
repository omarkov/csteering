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
 * @file voxel/generator/oct_gen.h
 * octal tree generator
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __OCT_GEN_H__
#define __OCT_GEN_H__

#include <cad/cad_model.h>
#include <generator/gen_help.h>
#include <octree/index_oct.h>

#include <node_index.h>
#include <exception.h>

#include <vector>

// ##### Exceptions ##################################################
/**
 * the CadModel contains no geometry data or it is impossible to build 
 * 3D objects based on the data 
 */
class BadModelException : public Exception {
    public:
    
    /**
     * exception constructor
     */
    BadModelException() : Exception("Bad CadModel") {
        std::cerr << "Bad model exception" << std::endl;
    }
    
    virtual ~BadModelException() {}
    
    virtual ErrorType getErrorType() {
    	return BAD_MODEL_ERROR;
    }
};

/**
 * the tree height is negative 
 */
class BadHeightException : public Exception {
    public:
    
    /**
     * exception constructor
     */
    BadHeightException() : Exception("Bad Height") {
        std::cerr << "The tree height can't be negative!" << std::endl;
    }
    
    virtual ~BadHeightException() {}
    
    virtual ErrorType getErrorType() {
    	return BAD_HEIGHT_ERROR;
    }
};
// ###### OctGen #####################################################
    
/**
 * octal tree generator
 */
class OctGen {
    public:
   
    /**
     * constructor
     * @param model CadModel that will be used for octal tree generation
     */
    OctGen(CadModel* model);
  
    /**
     * constructor
     * @param model CadModel used for octal tree generation
     * @param height height of the tree to be generated
     */
    OctGen(CadModel* model, Height height);
    
    /**
     * constructor
     * @param model CadModel used for octal tree generation
     * @param voxelSize voxel size of the tree to be generated
     */
    OctGen(CadModel* model, double voxelSize);
  
    /**
     * destructor
     */
    virtual OctGen::~OctGen();

    /**
     * inserts the point p into the octal tree
     * @param p node index of the point
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures
     */
    void add(NodeIndex p, Color color)
        throw (NotEnoughMemoryException*);

    /**
     * inserts the triangle \f$ \triangle p_Ap_Bp_C \f$
     * into the octal tree
     * @param pA node index of the corner point A
     * @param pB node index of the corner point B
     * @param pC node index of the corner point C
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structure 
     * @pre pA.getHeight() = pB.getHeight() = pC.getHeight()
     */
    void addTriangle(NodeIndex pA, NodeIndex pB, NodeIndex pC,
        Color color)
            throw (NotEnoughMemoryException*);

    /**
     * returns the CadModel used for the octal tree generation
     * @return CadModel
     * @post return != NULL
     */
    CadModel* getCadModel();

    #ifndef CLASSIC_MODE
  
    /**
     * returns the colour of the point p using the CadModel
     * @param p node index
     * @return NO_OBJECT if p is outside the object, the colour 
     *  of the neighbouring surface otherwise
     * @pre m_cadModel != NULL
     */
    Color getColor(NodeIndex p);
    
    #endif

    /**
     * returns the auxiliary object for the octal tree generation
     * @return GenHelp
     * @post return != NULL
     */
    GenHelp* getGenHelp();

    /**
     * returns the octal tree that is used for the generation
     * @return octal tree
     */
    IndexOct* getGenTree();

    /**
     * generates an octal tree of the maximal tree height maxTreeHeight 
     * if no such tree was previously generated or the flag 'changed' 
     * was set to 'true'
     * @param maxTreeHeight maximal tree height
     * @return generated octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */
    IndexOct* getOctree(Height maxTreeHeight)
        throw (NotEnoughMemoryException*, BadModelException*,  
            BadHeightException*);
    
    /**
     * generates an octal tree with the voxel size voxelSize 
     * if no such tree was previously generated or the flag 'changed' 
     * was set to 'true'
     * @param voxelSize voxel size
     * @return generated octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */    
    IndexOct* getOctree(double voxelSize)
        throw (NotEnoughMemoryException*, BadModelException*);
     
    /**
     * generates an octal tree of the maximal tree height used in the 
     * constructor or returns the already calculated octal tree if 
     * getOctree(Height maxTreeHeight) was called beforehand
     * @return octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */
    IndexOct* getOctree() 
        throw (NotEnoughMemoryException*, BadModelException*);
  
    /**
     * returns, whether the 'changed' flag was set to 'true' and therefore 
     * a new octal tree has to be calculated
     */
    bool getChanged();
  
    /**
     * sets the 'changed' flag to 'true' thus forcing generation 
     * of a new octal tree if any of the tree generation functions 
     * are called afterwards 
     * @param changed defines whether generation of new tree/voxelization 
     *  will be forced, if true - yes, otherwise - no
     */
    void setChanged(bool changed); 

    private:
 
    /**
     * generates an octal tree of the maximal tree height 
     * maxTreeHeight
     * @param maxTreeHeight maximal tree height
     * @return generated octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structure 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */
    IndexOct* genOctree(Height maxTreeHeight)
        throw (NotEnoughMemoryException*, BadModelException*, 
            BadHeightException*);
    
    /**
     * generates an octal tree with the previously defined parameters 
     * @return generated octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structure 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */        
    IndexOct* genOctree()
        throw (NotEnoughMemoryException*, BadModelException*);
    
    /**
     * generates an octal tree with the voxel size voxelSize 
     * @param voxelSize voxel size
     * @return generated octal tree
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structure 
     * @exception BadModelException the CadModel based on which the 
     *  octal tree generation was supposed to be done is incorrect
     */    
    IndexOct* genOctree(double voxelSize)
        throw (NotEnoughMemoryException*, BadModelException*);
    
    /**
     * adds line \f$\over{\mbox{pA pB}} \f$ of the colour color 
     * to the octal tree
     * @param start node index of the start point
     * @param end node index of the end point
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures  
     * @pre start.getHeight() = end.getHeight()
     */
    void addLine(NodeIndex start, NodeIndex end, Color color)
        throw (NotEnoughMemoryException*);

    /**
     * adds the CadObject object of the colour color to the octal tree
     * @param object CadObject
     * @param color colour
     * @exception NotEnoughMemoryException not enough memory to 
     *  allocate the new octal tree structures  
     * @pre object != NULL
     */
    void addObject(CadObject* object, Color color)
        throw (NotEnoughMemoryException*);

    /**
     * checks whether the CadModel is correct, if not a WrongModelException 
     * is thrown
     * @exception BadModelException m_cadModel == NULL 
     *  or else m_cadModel->empty() 
     *  or else not m_cadModel->getMinPoint() << m_cadModel->getMaxPoint()
     */
    void checkModel() throw (BadModelException*);

    /**
     * returns NO_OBJECT, if p is outside the object, the colour 
     *  of the neighbouring surface otherwise
     * @param p node index
     * @return colour of the point p
     * @pre m_cadModel != NULL
     * @pre p.getHeight() = BASE_NODE_HEIGHT
     */
    Color nextPointColor(NodeIndex p);

    #ifndef CLASSIC_MODE
    
    /**
     * returns NO_OBJECT, if p is outside the object, the colour 
     *  of the neighbouring surface otherwise, test ray method is used
     * @param p node index
     * @return colour of the point p
     * @pre m_cadModel != NULL
     * @pre p.getHeight() = BASE_NODE_HEIGHT
     * @pre RAY_AXIS < DIMENSIONS
     * @pre RAY_DIR = BACKWARD or RAY_DIR = FORWARD
     */
    Color testRayColor(NodeIndex p);
    
    #else

    /**
     * generates node using the classic algorithm
     * @param node index
     */
    void generateClassic(NodeIndex idx);

    /**
     * returns NO_OBJECT, if p is outside the object, the colour 
     *  of the neighbouring surface otherwise, test ray method is used
     * @param p node index
     * @param isAtBorder is set to 'true' if p belongs the surface of an object, 
     *  'false' otherwise
     * @return colour of the point p
     * @pre m_cadModel != NULL
     * @pre p.getHeight() = BASE_NODE_HEIGHT
     * @pre RAY_AXIS < DIMENSIONS
     * @pre RAY_DIR = BACKWARD or RAY_DIR = FORWARD
     */
    Color testRayColor(NodeIndex p, bool &isAtBorder);

    #endif

    /**
     * CadModel used for the generation
     */
    CadModel* m_cadModel;

    /**
     * auxiliary object for the octal tree structure generation
     */
    GenHelp* m_genHelp;

    /**
     * octal tree to be generated
     */
    IndexOct* m_octree;
    
    /**
     * maximal octal tree height
     */
    Height m_maxTreeHeight;
    
    /**
     * voxel size
     */
    double m_voxelSize;
  
    /**
     * defines, whether generation of new octal tree has to be forced 
     */
    bool m_changed;
};

#endif // ! __OCT_GEN_H__
