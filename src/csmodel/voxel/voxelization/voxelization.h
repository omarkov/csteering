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
 * @file: voxel/voxelization/voxelization.h
 * voxelization generator
 *
 * @author Mikhail Prokharau
 * @date 2004
 */
 
#ifndef __VOXELIZATION_H__
#define __VOXELIZATION_H__

#include <cad/cad_model.h>
#include <generator/oct_gen.h>
#include <reader/rawreader.h>
#include <../RemoteInterface.h>
#include <point.h>
#include <exception.h>

/**
 * class for generating voxelization
 */
class Voxelization {
 
public:
    
    /**
     * constructor, initializes the class objects by reading data
     * from a .raw-file and generating an octal tree of the given height 
     * and voxelization based on the previously extracted data
     * @param inFile name of the .raw-file
     * @param maxTreeHeight height of the tree to be generated
     * @param minPoint predefined minimal point of the CadModel
     * @param maxPoint predefined maximal point of the CadModel 
     * @param syncMinPoint minimal point of a larger model, with which 
     *  the octal tree to be generated is to be synchronized
     */
    Voxelization(const char* inFile, Height maxTreeHeight, 
        Point* minPoint = NULL, Point* maxPoint = NULL,
        Point* syncMinPoint = NULL);
    
    /**
     * constructor, initializes the class objects by reading data
     * from a .raw-file and generating an octal tree of the given height 
     * and voxelization based on the previously extracted data
     * @param inFile name of the .raw-file
     * @param voxelSize voxelSize of the tree to be generated
     * @param minPoint predefined minimal point of the CadModel
     * @param maxPoint predefined maximal point of the CadModel 
     * @param syncMinPoint minimal point of a larger model, with which 
     *  the octal tree to be generated is to be synchronized
     */
    Voxelization(const char* inFile, double voxelSize, Point* minPoint = NULL, 
        Point* maxPoint = NULL, Point* syncMinPoint = NULL);
    
    /**
     * constructor, initializes the class objects by reading
     * data from a triangulation structure and generating an octal 
     * tree of the given height and voxelization based on 
     * the previously extracted data
     * @param faces the triangulation structure array
     * @param maxTreeHeight height of the tree to be generated
     * @param size the triangulation structure array's size
     * @param minPoint predefined minimal point of the CadModel
     * @param maxPoint predefined maximal point of the CadModel 
     * @param syncMinPoint minimal point of a larger model, with which 
     *  the octal tree to be generated is to be synchronized
     */
    Voxelization(FACE_TRGL faces[], Height maxTreeHeight, int size,
        Point* minPoint = NULL, Point* maxPoint = NULL,
        Point* syncMinPoint = NULL);
    
    /**
     * constructor, initializes the class objects by reading
     * data from a triangulation structure and generating 
     * an octal tree of the given height and voxelization based on 
     * the previously extracted data
     * @param faces the triangulation structure array
     * @param voxelSize voxelSize of the tree to be generated
     * @param size the triangulation structure array's size
     * @param minPoint predefined minimal point of the CadModel
     * @param maxPoint predefined maximal point of the CadModel 
     * @param syncMinPoint minimal point of a larger model, with which 
     *  the octal tree to be generated is to be synchronized
     */
    Voxelization(FACE_TRGL faces[], double voxelSize, int size,
        Point* minPoint = NULL, Point* maxPoint = NULL,
        Point* syncMinPoint = NULL);
    
    /**
     * constructor, initializes the class objects by reading data
     * from a .raw-file, no automatic generation of an octal tree or 
     * voxelization takes place
     * @param inFile name of the .raw-file
     */
    Voxelization(const char* inFile);
    
    /**
     * constructor, initializes the class objects by reading
     * data from a triangulation structure, no automatic 
     * generation of an octal tree or voxelization takes place
     * @param faces the triangulation structure array
     * @param size the triangulation structure array's size
     * @param minPoint predefined minimal point of the CadModel
     * @param maxPoint predefined maximal point of the CadModel 
     * @param syncMinPoint minimal point of a larger model, with which 
     *  the octal tree to be generated is to be synchronized
     */
    Voxelization(FACE_TRGL faces[], int size, 
        Point* minPoint = NULL, Point* maxPoint = NULL, 
        Point* syncMinPoint = NULL);
    
    /**
     * destructor
     */
    virtual ~Voxelization();
    
    /**
     * returns the voxelization vector
     * @param extOctree external octal tree, based on which the voxelization 
     *  is to be calculated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    static Voxels* calcVoxels(IndexOct* extOctree, bool optimize = true, 
        bool alg = true);
    
    /**
     * calculates the adjusted minimal point
     * @param minPoint minimal point of the smaller CadModel
     * @param syncMinPoint minimal point of the larger CadModel, with which 
     *  the octal tree to be generated is to be synchronized
     * @param voxelSize voxelSize of the existing tree and the tree 
     *  to be generated
     * @return the adjusted minimal point
     */
    static Point calcAdjMinPoint(Point minPoint, Point syncMinPoint, 
        double voxelSize);
    
    /**
     * returns the voxelization vector
     * @param inFile name of the .raw-file
     * @param maxTreeHeight height of the tree to be generated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    Voxels* getVoxels(const char* inFile, Height maxTreeHeight, 
        bool optimize = true, bool alg = true);

    
    /**
     * returns the voxelization vector
     * @param inFile name of the .raw-file
     * @param voxelSize voxelSize of the tree to be generated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */    
    Voxels* getVoxels(const char* inFile, double voxelSize, 
        bool optimize = true, bool alg = true);
    
    /**
     * returns the voxelization vector
     * @param faces the triangulation structure array
     * @param maxTreeHeight height of the tree to be generated
     * @param size the triangulation structure array's size
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    Voxels* getVoxels(FACE_TRGL faces[], Height maxTreeHeight, int size, 
        bool optimize = true, bool alg = true);

    /**
     * returns the voxelization vector, new calculation is always forced
     * @param faces the triangulation structure array
     * @param maxTreeHeight height of the tree to be generated
     * @param size the triangulation structure array's size
     * @param min the forced minimal point of the CadModel
     * @param max the forced maximal point of the CadModel
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    Voxels* getVoxels(FACE_TRGL faces[], Height maxTreeHeight, int size, 
        Point min, Point max, bool optimize = true, bool alg = true);
    
    /**
     * returns the voxelization vector
     * @param faces the triangulation structure array
     * @param voxelSize voxelSize of the tree to be generated
     * @param size the triangulation structure array's size
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */    
    Voxels* getVoxels(FACE_TRGL faces[], double voxelSize, int size, 
        bool optimize = true, bool alg = true);
    
    /**
     * returns the voxelization vector
     * @param maxTreeHeight height of the tree to be generated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    Voxels* getVoxels(Height maxTreeHeight, bool optimize = true, 
        bool alg = true);
    
    /**
     * returns the voxelization vector
     * @param voxelSize voxelSize of the tree to be generated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */    
    Voxels* getVoxels(double voxelSize, bool optimize = true, 
        bool alg = true);    
    
    /**
     * returns the previously calculated voxelization vector 
     * @return the previously calculated voxelization vector 
     *  as defined in utils/general.h, 
     *  NULL if no voxelization vector was previously calculated
     */
    Voxels* getVoxels();
    
    /**
     * returns the octal tree
     * @param inFile name of the .raw-file
     * @param maxTreeHeight height of the tree to be generated
     * @return the octal tree as defined in octree/index_oct.*
     */
    IndexOct* getOctree(const char* inFile, Height maxTreeHeight);
    
    /**
     * returns the octal tree
     * @param inFile name of the .raw-file
     * @param voxelSize voxelSize of the tree to be generated
     * @return the octal tree as defined in octree/index_oct.*
     */
    IndexOct* getOctree(const char* inFile, double voxelSize);
    
    /**
     * returns the octal tree
     * @param faces the triangulation structure array
     * @param maxTreeHeight height of the tree to be generated
     * @param size the triangulation structure array's size
     * @return the octal tree as defined in octree/index_oct.*
     */
    IndexOct* getOctree(FACE_TRGL faces[], Height maxTreeHeight, 
        int size);
    
    /**
     * returns the octal tree
     * @param faces the triangulation structure array
     * @param voxelSize voxelSize of the tree to be generated
     * @param size the triangulation structure array's size
     * @return the octal tree as defined in octree/index_oct.*
     */    
    IndexOct* getOctree(FACE_TRGL faces[], double voxelSize, 
        int size);    
    
    /**
     * returns the octal tree
     * @param maxTreeHeight height of the tree to be generated
     * @return the octal tree as defined in octree/index_oct.*
     */
    IndexOct* getOctree(Height maxTreeHeight);
    
    /**
     * returns the octal tree
     * @param voxelSize voxelSize of the tree to be generated
     * @return the octal tree as defined in octree/index_oct.*
     */
    IndexOct* getOctree(double voxelSize);
    
    /**
     * returns the previously calculated octal tree
     * @return the previously calculated octal tree 
     *  as defined in utils/general.h, 
     *  NULL if no octal tree was previously calculated
     */
    IndexOct* getOctree();
    
    /**
     * returns the voxel size
     */
    double getVoxelSize();    
    
    /**
     * returns the point in 3d space corresponding to the voxelization 
     * point defined by its x, y, z coordinates
     * @param x the x-coordinate in the voxelization vector
     * @param y the y-coordinate in the voxelization vector
     * @param z the z-coordinate in the voxelization vector
     * @return the point in 3d space corresponding to the voxelization 
     *  point as defined in cad/objects/point.*, null Point is returned
     *  if no octal tree was generated yet
     */
    Point getPoint(int x, int y, int z);
    
    /**
     * returns the point in 3d space corresponding to the voxelization 
     * point defined by its x, y, z coordinates
     * @param gp an octree/voxelization point, as defined in geom/geom_point.*
     * @return the point in 3d space corresponding to the voxelization 
     *  point as defined in cad/objects/point.*, null Point is returned
     *  if no octal tree was generated yet
     */
    Point getPoint(GeomPoint gp);
    
    /**
     * returns the octree/voxelization point corresponding to the point 
     * in 3d space defined by its x, y, z coordinates
     * @param x the x-coordinate in 3d space
     * @param y the y-coordinate in 3d space
     * @param z the z-coordinate in 3d space
     * @return the octree/voxelization point corresponding to the point 
     *  in 3d space as defined in geom/geom_point.*, null GeomPoint is returned
     *  if no octal tree was generated yet
     */
    GeomPoint getGeomPoint(double x, double y, double z);
    
    /**
     * returns the octree/voxelization node index corresponding to the point 
     * in 3d space defined by its x, y, z coordinates
     * @param x the x-coordinate in 3d space
     * @param y the y-coordinate in 3d space
     * @param z the z-coordinate in 3d space
     * @return the octree/voxelization node index corresponding to the point 
     *  in 3d space as defined in geom/node_index.*, null NodeIndex is returned
     *  if no octal tree was generated yet
     */
    NodeIndex getNodeIndex(double x, double y, double z);
    
    /**
     * returns the octree/voxelization point corresponding to the point 
     * in 3d space defined by its x, y, z coordinates
     * @param p a 3d space point as defined in cad/objects/point.*
     * @return the octree/voxelization point corresponding to the point 
     *  in 3d space as defined in geom/geom_point.*, null GeomPoint is returned
     *  if no octal tree was generated yet
     */
    GeomPoint getGeomPoint(Point p);
    
    /**
     * returns the maximal octree/voxelization point 
     * @return the maximal octree/voxelization point 
     *  as defined in geom/geom_point.*, null GeomPoint is returned
     *  if no octal tree was generated yet 
     */
    GeomPoint getMaxGeomPoint();
    
    /**
     * returns the minimal octree/voxelization point 
     * @return the minimal octree/voxelization point 
     *  as defined in geom/geom_point.*, null GeomPoint is returned
     *  if no octal tree was generated yet
     */
    GeomPoint getMinGeomPoint();
    
    /**
     * returns the maximal 3d point in space
     * @return the minimal 3d point in space as defined cad/objects/point.*,
     *  null Point is returned if no octal tree was generated yet
     */
    Point getMaxPoint();
    
    /**
     * returns the minimal 3d point in space
     * @return the minimal 3d point in space as defined cad/objects/point.*, 
     *  null Point is returned if no octal tree was generated yet
     */
    Point getMinPoint();
    
    /**
     * whether the 'changed' flag was set to 'true' and therefore 
     * a new octal tree and voxelization have to be calculated
     */
    bool getChanged();
  
    /**
     * sets the 'changed' flag to 'true' thus forcing generation 
     * of new octal tree and voxelization if any of the tree/voxelization 
     * generation functions are called afterwards 
     * @param changed defines whether generation of new tree/voxelization 
     *  will be forced, if true - yes, otherwise - no
     */
    void setChanged(bool changed); 
	
	/**
	 * calculates the CadModel
	 */
    CadModel* calcCadModel();
	
	/**
	 * returns the CadModel
	 */
    CadModel* getCadModel();
    
private:
    
    /**
     * initializes the internal structures based on the data passed by a 
     * properly initialized RawReader object
     */
    void init();
    
    /**
     * initializes the internal structures based on the data passed by a 
     * properly initialized RawReader object
     * @param maxTreeHeight height of the tree to be generated
     */
    void init(Height maxTreeHeight);
    
    /**
     * initializes the internal structures based on the data passed by a 
     * properly initialized RawReader object
     * @param maxTreeHeight height of the tree to be generated
     * @param min the forced minimal point of the CadModel
     * @param max the forced maximal point of the CadModel
     */
    void init(Height maxTreeHeight, Point min, Point max);
    
    /**
     * initializes the internal structures based on the data passed by a 
     * properly initialized RawReader object
     * @param voxelSize voxelSize of the tree to be generated
     */
    void init(double voxelSize);
    
    /**
     * initializes the internal structures based on the data read from 
     * the passed .raw-file
     * @param inFile name of the .raw-file
     * @param maxTreeHeight height of the tree to be generated
     */
    void init(const char* inFile, Height maxTreeHeight);
    
    /**
     * initializes the internal structures based on the data read from 
     * the passed .raw-file
     * @param inFile name of the .raw-file
     * @param voxelSize voxelSize of the tree to be generated
     */
    void init(const char* inFile, double voxelSize);
    
    /**
     * initializes the internal structures based on the passed 
     * triangulation structures 
     * @param faces the triangulation structure array
     * @param size the triangulation structure array's size
     * @param maxTreeHeight height of the tree to be generated
     */
    void init(FACE_TRGL faces[], Height maxTreeHeight, int size);
    
    /**
     * initializes the internal structures based on the passed 
     * triangulation structures 
     * @param faces the triangulation structure array
     * @param size the triangulation structure array's size
     * @param maxTreeHeight height of the tree to be generated
     * @param min the forced minimal point of the CadModel
     * @param max the forced maximal point of the CadModel
     */
    void init(FACE_TRGL faces[], Height maxTreeHeight, int size, 
        Point min, Point max);
    
    /**
     * initializes the internal structures based on the passed 
     * triangulation structures 
     * @param faces the triangulation structure array
     * @param size the triangulation structure array's size
     * @param voxelSize voxelSize of the tree to be generated
     */
    void init(FACE_TRGL faces[], double voxelSize, int size);
    
    /**
     * processes a node defined by its position and index
     * @param voxels the voxelization array to be processed
     * @param tree an octal tree
     * @param root the root of the passed tree
     * @param node the index of the passed node
     */
    static void processNode(Voxels* voxels, IndexOct* tree, 
        OctStruct::_octree& root, NodeIndex node);
    
    /**
     * fills the below lying nodes of an octal tree in the voxelization array
     * @param voxels the voxelization array to be processed
     * @param tree the root of the subtree to be filled
     * @param node the index of the passed subtree root
     */
    static void fill(Voxels* voxels, IndexOct* tree, NodeIndex node);
    
    /**
     * generates voxelization based on properly initialized internal 
     * structures
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    Voxels* intVoxelize(bool optimize = true, bool alg = true);
    
    /**
     * generates voxelization based on properly initialized internal 
     * structures
     * @param voxelOctree octal tree, based on which the voxelization 
     *  is to be calculated
     * @param optimize defines, whether optimal voxelization is used,
     *  if true then the dimension subvectors are optimally configured,
     *  if false then the whole voxelization is returned as defined by the
     *          relevant octal tree
     * @param alg defines, which algorithm is used to fill the voxelization
     *  vector,
     *  if true then an optimal algorithm based on internal octal tree
     *          structure is used
     *  if false then an algorithm based on standard library functions is used
     * @return the voxelization vector as defined in utils/general.h
     */
    static Voxels* voxelize(IndexOct* voxelOctree, bool optimize = true, 
        bool alg = true);
 
    /**
     * name of the .raw file to be processed
     */
    const char* m_inFile;
    
    /**
     * triangulation array
     */
    FACE_TRGL* m_faces;
    
    /**
     * size of the triangulation array
     */
    int m_faces_size;
    
    /**
     * raw file or data reader
     */ 
    RawReader* m_rawreader;
    
    /**
     * maximal octal tree height 
     */
    Height m_maxTreeHeight;
    
    /**
     * voxel size
     */
    double m_voxelSize;
    
    /**
     * defines, which voxelization algorithm is to be used
     */
    bool m_optimize;
    
    /**
     * internal CadModel
     */
    CadModel* m_cadModel;
    
    /**
     * internal OctGen
     */
    OctGen* m_octGen;
    
    /**
     * internal voxelization vector
     */
    Voxels* m_voxels;
    
    /**
     * internal octal tree
     */
    IndexOct* m_octree;
    
    /**
     * defines, whether generation of new octal tree / voxelization 
     * has to be forced (i.e. there were changes to external 
     * files/triangulation structires)
     */
    bool m_changed;
    
    /**
     * the minimal point of the CadModel
     */
    Point* m_minPoint;
    
    /**
     * the maximal point of the CadModel
     */
    Point* m_maxPoint;
    
    /**
     * minimal point of a larger model, with which 
     * the octal tree is to be synchronized
     */
    Point* m_syncMinPoint;
 
}; // class Voxelization

// ##### Exceptions ################################################

/**
 * is thrown in case of a general voxelization error
 */
class VoxelExc : public Exception {
    public:
    
    VoxelExc() : Exception("General voxelization error") {}
    
    VoxelExc(std::string msg) : Exception (msg) {}
    
    virtual ~VoxelExc() {}
    
    virtual ErrorType getErrorType() {
    	return VOXEL_ERROR;
    }
};

/**
 * is thrown in case of a voxelization error caused by an improperly initialized
 * RawReader class object
 */
class RawReaderVoxelExc: public VoxelExc {
    public:
    
    RawReaderVoxelExc() : VoxelExc("Raw reader voxelization error") {}
    
    virtual ~RawReaderVoxelExc() {}
    
    virtual ErrorType getErrorType() {
    	return RAWREADER_VOXEL_ERROR;
    }
};

/**
 * is thrown in case of a voxelization error caused by an improperly initialized
 * Reader class object
 */
class ReaderVoxelExc: public VoxelExc {
    public:
    
    ReaderVoxelExc() : VoxelExc("Reader voxelization error") {}
    
    virtual ~ReaderVoxelExc() {}
    
    virtual ErrorType getErrorType() {
    	return READER_VOXEL_ERROR;
    }
};

/**
 * is thrown in case of a voxelization error caused by an improperly initialized
 * CadModel class object
 */
class ModelVoxelExc: public VoxelExc {
    public:
    
    ModelVoxelExc() : VoxelExc("CadModel voxelization error") {}
    
    virtual ~ModelVoxelExc() {}
    
    virtual ErrorType getErrorType() {
    	return MODEL_VOXEL_ERROR;
    }
};

/**
 * is thrown in case of a voxelization error caused by an improperly initialized
 * OctGen class object
 */
class OctGenVoxelExc: public VoxelExc {
    public:
    
    OctGenVoxelExc() : VoxelExc("Octal tree generator voxelization error") {}
    
    virtual ~OctGenVoxelExc() {}
    
    virtual ErrorType getErrorType() {
    	return OCTGEN_VOXEL_ERROR;
    }
};

#endif // ! __VOXELIZATION_H__
