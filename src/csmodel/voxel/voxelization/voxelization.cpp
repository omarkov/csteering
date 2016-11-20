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
 * file: voxel/voxelization/voxelization.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */
 
#include <iostream>
#include <string>
#include <voxelization/voxelization.h>
#include <reader/reader.h>
#include <generator/oct_gen.h>
#include <filename.h>
#include <timer.h>

using namespace std;

// ##### Voxelization() ##############################################
Voxelization::Voxelization(const char* inFile, 
        Height maxTreeHeight, Point* minPoint, Point* maxPoint,
        Point* syncMinPoint){

    m_inFile = inFile;
    m_faces = NULL;
    m_faces_size = 0;
    m_voxelSize = 1.0;
    m_maxTreeHeight = maxTreeHeight;
    m_octGen = NULL;
    m_cadModel = NULL;
    m_optimize = true;
    m_rawreader = NULL; 
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_syncMinPoint = syncMinPoint;
    
    m_octree = getOctree(inFile, maxTreeHeight);
    m_voxels = getVoxels(inFile, maxTreeHeight);
    m_changed = false;
}

// ##### Voxelization() ##############################################
Voxelization::Voxelization(const char* inFile, double voxelSize, 
    Point* minPoint, Point* maxPoint, Point* syncMinPoint){

    m_inFile = inFile;
    m_faces = NULL;
    m_faces_size = 0;
    m_maxTreeHeight = 0;
    m_voxelSize = voxelSize;
    m_octGen = NULL;
    m_cadModel = NULL;
    m_optimize = true;
    m_rawreader = NULL;
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_syncMinPoint = syncMinPoint;
    
    m_octree = getOctree(inFile, voxelSize);
    m_voxels = getVoxels(inFile, voxelSize);
    m_changed = false;
}

// ##### Voxelization() ##############################################
Voxelization::Voxelization(FACE_TRGL faces[], Height maxTreeHeight,
        int size, Point* minPoint, Point* maxPoint, Point* syncMinPoint){
    m_inFile = NULL;
    m_faces = faces;
    m_faces_size = size;
    m_maxTreeHeight = maxTreeHeight;
    m_voxelSize = 1.0;
    m_octGen = NULL;
    m_voxels = NULL;
    m_cadModel = NULL;
    m_optimize = true;
    m_octree = NULL;
    m_rawreader = NULL;
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_syncMinPoint = syncMinPoint;
    
    m_octree = getOctree(faces, maxTreeHeight, size);
    m_voxels = getVoxels(faces, maxTreeHeight, size);
    m_changed = false;
}

// ##### Voxelization() ##############################################
Voxelization::Voxelization(FACE_TRGL faces[], double voxelSize,
        int size, Point* minPoint, Point* maxPoint, Point* syncMinPoint){
    m_inFile = NULL;
    m_faces = faces;
    m_faces_size = size;
    m_maxTreeHeight = 0;
    m_voxelSize = voxelSize;
    m_octGen = NULL;
    m_octree = NULL;
    m_cadModel = NULL;
    m_optimize = true;
    m_voxels = NULL;
    m_rawreader = NULL;
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_syncMinPoint = syncMinPoint;
    
    m_octree = getOctree(faces, voxelSize, size);
    m_voxels = getVoxels(faces, voxelSize, size);
    m_changed = false;
}

// ##### Voxelization() ##############################################
Voxelization::Voxelization(const char* inFile){
    m_inFile = inFile;
    m_faces = NULL;
    m_faces_size = 0;
    m_maxTreeHeight = 0;
    m_voxelSize = 1.0;
    m_optimize = true;
    m_octGen = NULL;
    m_cadModel = NULL;
    m_changed = true;
    m_octree = NULL;
    m_voxels = NULL;
    m_minPoint = NULL;
    m_maxPoint = NULL;
    m_syncMinPoint = NULL;
    m_rawreader = NULL;
}

// ##### Voxelization() ##############################################
Voxelization::Voxelization(FACE_TRGL faces[], int size, Point* minPoint, 
    Point* maxPoint, Point* syncMinPoint){
    	
    m_inFile = NULL;
    m_faces = faces;
    m_faces_size = size;
    m_maxTreeHeight = 0;
    m_voxelSize = 1.0;
    m_optimize = true;
    m_rawreader = NULL;
    
    m_minPoint = minPoint;
    m_maxPoint = maxPoint;
    m_syncMinPoint = syncMinPoint;
    
    m_octree = NULL;
    m_voxels = NULL;
    m_octGen = NULL;
    m_cadModel = NULL;
    m_changed = true;
}

// ##### ~Voxelization() #############################################
Voxelization::~Voxelization(){
    delete m_octGen;
    delete m_cadModel;
    //delete m_voxels;
    //delete m_octree;
    delete m_rawreader;
    
    if (m_minPoint != NULL)
        delete m_minPoint;
        
    if (m_maxPoint != NULL)
        delete m_maxPoint;
        
    if (m_syncMinPoint != NULL)
        delete m_syncMinPoint;
}

// ##### calcCadModel() ###############################################
CadModel *Voxelization::calcCadModel()
{
	if ((m_faces == NULL) || (m_faces_size == 0)) throw VoxelExc();
	
	if (m_rawreader != NULL) delete m_rawreader;
	
    m_rawreader = new RawReader(m_faces, m_faces_size);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
    
    Reader* reader = new Reader(m_rawreader->getTriangles());
    
    if (reader == NULL) throw ReaderVoxelExc();
    
    cout << "Extracting data..." << endl;
    
    m_cadModel = reader->getCadModel();
	
    return m_cadModel;
}

// ##### getCadModel() ###############################################
CadModel *Voxelization::getCadModel()
{
    return m_cadModel;
}
 
// ##### init() ######################################################
void Voxelization::init (const char* inFile, Height maxTreeHeight) {
    
    if (m_rawreader != NULL) delete m_rawreader;
    
    m_rawreader = new RawReader(inFile);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
        
    init(maxTreeHeight);
}
 
// ##### init() ######################################################
void Voxelization::init (const char* inFile, double voxelSize) {
	
	if (m_rawreader != NULL) delete m_rawreader;
	
    m_rawreader = new RawReader(inFile);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
        
    init(voxelSize);
} 
 
// ##### init() ######################################################
void Voxelization::init(FACE_TRGL faces[], Height maxTreeHeight, int size){
	
    if (m_rawreader != NULL) delete m_rawreader;
	
    m_rawreader = new RawReader(faces, size);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
    
    init(maxTreeHeight);
} 

// ##### init() ######################################################
void Voxelization::init(FACE_TRGL faces[], Height maxTreeHeight, int size, 
    Point min, Point max){
	
    if (m_rawreader != NULL) delete m_rawreader;
	
    m_rawreader = new RawReader(faces, size);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
    
    init(maxTreeHeight, min, max);
} 

// ##### init() ######################################################
void Voxelization::init(FACE_TRGL faces[], double voxelSize, int size){

	if (m_rawreader != NULL) delete m_rawreader;
	
    m_rawreader = new RawReader(faces, size);
    
    if (m_rawreader == NULL) throw RawReaderVoxelExc();
    
    init(voxelSize);
} 
 
// ##### init() ######################################################
void Voxelization::init() {
	
    Reader* reader = new Reader(m_rawreader->getTriangles());
    
    if (reader == NULL) throw ReaderVoxelExc();
    
    cout << "Extracting data..." << endl;
    
    m_cadModel = reader->getCadModel();
    
    if (m_cadModel == NULL) throw ModelVoxelExc();
    
    if ((m_syncMinPoint != NULL) && (m_minPoint == NULL)){	
        m_minPoint = new Point(m_cadModel->getMinPoint());
    }
        
    try {
        
        if (m_minPoint != NULL) 
            m_cadModel->setMinPoint(*m_minPoint);
        
        if (m_maxPoint != NULL)
            m_cadModel->setMaxPoint(*m_maxPoint);
    
        if ((m_syncMinPoint != NULL) &&  (m_minPoint != NULL)){
        	cout << "Adjusting minimal point..." << endl;
        	
    	    Point newMinPoint;
    	
    	    for (int i = 0; i < DIMENSIONS; i++)
    	        newMinPoint.setCoordinate(i, (floor (((*m_minPoint)[i] 
    	            - (*m_syncMinPoint)[i]) / m_voxelSize) 
    	            * m_voxelSize) + (*m_syncMinPoint)[i]);
    	    
    	    cout << "The original minimal point: ";
    	    m_minPoint->print();
    	    cout << endl;
    	    
    	    
    	    cout << "The newly adjusted minimal point: ";
    	    newMinPoint.print();
    	    cout << endl;
    	    
    	    m_cadModel->setMinPoint(newMinPoint);
        }
    }
    
    catch (ModelExc &e) {
		cerr << "Model exception, update failed" << endl;
	}
    
    delete reader;
    
    cout << "Generating octal tree structures..." << endl;

	m_octree = NULL;

    if (m_octGen != NULL) delete m_octGen;
}

// ##### init() ######################################################
void Voxelization::init(Height maxTreeHeight, Point min, Point max) {
    
    init();

    getCadModel()->setMinPoint(min);
    getCadModel()->setMaxPoint(max);
        

    m_octGen = new OctGen(m_cadModel, maxTreeHeight);
    
    if (m_octGen == NULL) throw OctGenVoxelExc();
    
    m_octree = m_octGen->getOctree();
}

// ##### init() ######################################################
void Voxelization::init(Height maxTreeHeight) {
    
    init();
        

    m_octGen = new OctGen(m_cadModel, maxTreeHeight);
    
    if (m_octGen == NULL) throw OctGenVoxelExc();
    
    m_octree = m_octGen->getOctree();
}

// ##### init() ######################################################
void Voxelization::init(double voxelSize) {
    
    init();
        
    m_octGen = new OctGen(m_cadModel, voxelSize);
    
    if (m_octGen == NULL) throw OctGenVoxelExc();
    
    m_octree = m_octGen->getOctree();
}

// ##### calcVoxels() ################################################
Voxels* Voxelization::calcVoxels(IndexOct* extOctree, bool optimize, 
        bool alg){
    return voxelize(extOctree, optimize, alg); 
}

// ##### calcAdjMinPoint() ###########################################
Point Voxelization::calcAdjMinPoint(Point minPoint, Point syncMinPoint, 
    double voxelSize){
	
    Point adjMinPoint;
    	
    for (int i = 0; i < DIMENSIONS; i++)
        adjMinPoint.setCoordinate(i, (floor ((minPoint[i] 
    	    - syncMinPoint[i]) / voxelSize) 
    	    * voxelSize) + syncMinPoint[i]);
    	    
    return adjMinPoint;
}

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(const char* inFile, 
        Height maxTreeHeight, bool optimize, bool alg){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || (strcmp(m_inFile, inFile) != 0) 
    || (m_maxTreeHeight != maxTreeHeight)) {
        m_faces = NULL;
        m_faces_size = 0;
        init (inFile, maxTreeHeight);
        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }

    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
};

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(const char* inFile, 
        double voxelSize, bool optimize, bool alg){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || (strcmp(m_inFile, inFile) != 0) 
    || ((m_voxelSize - voxelSize) > EPSILON)) {
        m_faces = NULL;
        m_faces_size = 0;
        init (inFile, voxelSize);
        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }

    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
};

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(Height maxTreeHeight, bool optimize, 
    bool alg){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || (m_maxTreeHeight != maxTreeHeight)) {

        if (m_inFile != NULL)
            init (m_inFile, maxTreeHeight);
        else
            init (m_faces, maxTreeHeight, m_faces_size);

        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }

    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
};

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(double voxelSize, bool optimize, 
    bool alg){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || ((m_voxelSize - voxelSize) > EPSILON)) {

		cout << "test" << endl;

        if (m_inFile != NULL)
            init (m_inFile, voxelSize);
        else
            init (m_faces, voxelSize, m_faces_size);

        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }

    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
};

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(FACE_TRGL faces[], Height maxTreeHeight, 
        int size, bool optimize, bool alg){
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || (m_faces != faces) 
    || (m_maxTreeHeight != maxTreeHeight)) {
        m_inFile = NULL;
        m_faces_size = size;
        init (faces, maxTreeHeight, size);
        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }
    
    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
}

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(FACE_TRGL faces[], Height maxTreeHeight, 
    int size, Point min, Point max, bool optimize, bool alg){
    
    m_inFile = NULL;
    m_faces_size = size;
    init (faces, maxTreeHeight, size, min, max);
    m_changed = false;
    m_optimize = optimize;
    if (m_voxels != NULL) delete m_voxels;
    m_voxels = intVoxelize(optimize, alg);
    
    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
}

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels(FACE_TRGL faces[], double voxelSize, 
        int size, bool optimize, bool alg){
    if ((m_octGen == NULL) || (m_changed)
    || (m_optimize != optimize)
    || (m_faces != faces) 
    || ((m_voxelSize - voxelSize) > EPSILON)) {
        m_inFile = NULL;
        m_faces_size = size;
        init (faces, voxelSize, size);
        m_changed = false;
        m_optimize = optimize;
        if (m_voxels != NULL) delete m_voxels;
        m_voxels = intVoxelize(optimize, alg);
    }
    
    if (m_voxels == NULL) {
        m_voxels = intVoxelize(optimize, alg);
    }
        
    if (m_voxels == NULL) throw VoxelExc();
    
    return m_voxels;
}

// ##### getVoxels() #################################################
Voxels* Voxelization::getVoxels() {
    if ((m_voxels != NULL) && (!m_changed))
        return m_voxels;
    
    return NULL;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(const char* inFile, 
        Height maxTreeHeight){
    
    if ((m_octGen == NULL) || (m_changed)
    || (strcmp(m_inFile, inFile) != 0) 
    || (m_maxTreeHeight != maxTreeHeight)) {
        m_faces = NULL;
        m_faces_size = 0;
        init (inFile, maxTreeHeight);
        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(maxTreeHeight);
    }
        
    if (m_octree == NULL) throw VoxelExc();
    
    return m_octree;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(const char* inFile, 
        double voxelSize){
    
    if ((m_octGen == NULL) || (m_changed)
    || (strcmp(m_inFile, inFile) != 0) 
    || ((m_voxelSize - voxelSize) > EPSILON)) {
        m_faces = NULL;
        m_faces_size = 0;
        init (inFile, voxelSize);
        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(voxelSize);
    }
        
    if (m_octree == NULL) throw VoxelExc();
    
    return m_octree;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(FACE_TRGL faces[], 
        Height maxTreeHeight, int size){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_faces != faces)
    || (m_maxTreeHeight != maxTreeHeight)) {
        m_inFile = NULL;
        m_faces_size = size;
        init (faces, maxTreeHeight, size);
        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(maxTreeHeight);
    }
        
    if (m_octree == NULL) throw VoxelExc();
        
    return m_octree;
};

// ##### getVoxelSize() ########################################
double Voxelization::getVoxelSize()
{
    if(m_octGen)
        return m_octGen->getGenHelp()->getVoxelSize();
    else
        return 1.0;
}

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(FACE_TRGL faces[], 
        double voxelSize, int size){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_faces != faces)
    || ((m_voxelSize - voxelSize) > EPSILON)) {
        m_inFile = NULL;
        m_faces_size = size;
        init (faces, voxelSize, size);
        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(voxelSize);
    }
        
    if (m_octree == NULL) throw VoxelExc();
        
    return m_octree;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(Height maxTreeHeight){
    
    if ((m_octGen == NULL) || (m_changed)
    || (m_maxTreeHeight != maxTreeHeight)) {

        if (m_inFile != NULL)
            init (m_inFile, maxTreeHeight);
        else
            init (m_faces, maxTreeHeight, m_faces_size);

        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(maxTreeHeight);
    }
        
    if (m_octree == NULL) throw VoxelExc();
    
    return m_octree;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(double voxelSize){
    
    if ((m_octGen == NULL) || (m_changed)
    || ((m_voxelSize - voxelSize) > EPSILON)) {

        if (m_inFile != NULL)
            init (m_inFile, voxelSize);
        else
            init (m_faces, voxelSize, m_faces_size);

        m_changed = false;
    }
    
    if (m_octree == NULL) {
        m_octree = m_octGen->getOctree(voxelSize);
    }
        
    if (m_octree == NULL) throw VoxelExc();
    
    return m_octree;
};

// ##### getOctree() #################################################
IndexOct* Voxelization::getOctree(){
    if ((m_octree != NULL) && (!m_changed))     
        return m_octree;
    
    return NULL;
};

// ##### getPoint() ##################################################
Point Voxelization::getPoint(int x, int y, int z){

    if (m_octGen == NULL) 
        return Point(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    GeomPoint gp = GeomPoint(NodeIndex(x, y, z));   
    Point p = genHelp->getCadPoint(gp);
    
    return p;

};

// ##### getPoint() ##################################################
Point Voxelization::getPoint(GeomPoint gp){

    if (m_octGen == NULL) 
        return Point(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    Point p = genHelp->getCadPoint(gp);
    
    return p;

};

// ##### getMaxPoint() ###############################################
Point Voxelization::getMaxPoint(){

    if (m_cadModel == NULL) 
        return Point(0, 0, 0);
    
    return m_cadModel->getMaxPoint();
}
    
// ##### getMinPoint() ###############################################
Point Voxelization::getMinPoint(){
    if (m_cadModel == NULL) 
        return Point (0, 0, 0);
    
    return m_cadModel->getMinPoint();
}

// ##### getMaxGeomPoint() ###########################################
GeomPoint Voxelization::getMaxGeomPoint(){
    if (m_octGen == NULL) 
        return GeomPoint(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    GeomPoint gp = genHelp->getGeomPoint(getMaxPoint());
    
    return gp;
}
    
// ##### getMinGeomPoint() ###########################################
GeomPoint Voxelization::getMinGeomPoint(){
    if (m_octGen == NULL) 
        return GeomPoint(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    GeomPoint gp = genHelp->getGeomPoint(getMinPoint());
    
    return gp;
}

// ##### getGeomPoint() ##############################################
GeomPoint Voxelization::getGeomPoint(double x, double y, double z){
    if (m_octGen == NULL) 
        return GeomPoint(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    Point p = Point(x, y, z);   
    GeomPoint gp = genHelp->getGeomPoint(p);
    
    return gp;
}

// ##### getNodeIndex() ##############################################
NodeIndex Voxelization::getNodeIndex(double x, double y, double z)
{
    if (m_octGen == NULL) 
        return NodeIndex(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();
    Point p = Point(x, y, z);   
 
    return genHelp->getNodeIndex(p);  
}

// ##### getGeomPoint() ##############################################
GeomPoint Voxelization::getGeomPoint(Point p){
    if (m_octGen == NULL) 
        return GeomPoint(0, 0, 0);
    
    GenHelp* genHelp = m_octGen->getGenHelp();                  
    GeomPoint gp = genHelp->getGeomPoint(p);
    
    return gp;
}

// ##### getChanged() ################################################
bool Voxelization::getChanged() {
    return m_changed;
};
  
// ##### setChanged() ################################################
void Voxelization::setChanged(bool changed){
    m_changed = changed;
};

// ##### intVoxelize() ###############################################
Voxels* Voxelization::intVoxelize(bool optimize, bool alg) {
    Voxels* voxels = voxelize(m_octree, optimize, alg);
    return voxels;
}

// ##### voxelize() ##################################################
Voxels* Voxelization::voxelize(IndexOct* voxelOctree, bool optimize, bool alg) {

    Height maxTreeHeight = voxelOctree->getMaxTreeHeight();
    
    Voxels* voxels = new Voxels;
    Timer timer;
    
    int x_max, y_max, z_max;
    int x_min, y_min, z_min;
    
    cerr << "Generating the voxelization... " << endl;
    timer.reset();      
    
    if (optimize) {
        GeomPoint maxGeomPoint = voxelOctree->getMaxGeomPoint();
        GeomPoint minGeomPoint = voxelOctree->getMinGeomPoint();
        
        x_max = static_cast<int>(floor(maxGeomPoint.getX()));
        y_max = static_cast<int>(floor(maxGeomPoint.getY()));
        z_max = static_cast<int>(floor(maxGeomPoint.getZ()));
        
        x_min = static_cast<int>(floor(minGeomPoint.getX()));
        y_min = static_cast<int>(floor(minGeomPoint.getY()));
        z_min = static_cast<int>(floor(minGeomPoint.getZ()));
    
        
    } else {
        
        int max = (1 << maxTreeHeight) - 1;
        x_max = max;
        y_max = max;
        z_max = max;
        x_min = 0;
        y_min = 0;
        z_min = 0;
    }

    voxels->resize(x_max + 1);
    
    for (int x = x_min; x <= x_max; ++x) {
        
        (*voxels)[x].resize(y_max + 1);
        
        for (int y = y_min; y <= y_max; ++y) {
            
            (*voxels)[x][y].resize(z_max + 1);
            
            for (int z = z_min; z <= z_max; ++z) {

                if (alg) {
                    (*voxels)[x][y][z] = false;
                } else {
    
                    if ((voxelOctree->getColor(NodeIndex(x, y, z))) != 0) 
                        (*voxels)[x][y][z] = true;
                    else
                        (*voxels)[x][y][z] = false;
                }
            }
        }
    }
    
    OctStruct::_octree& root = voxelOctree->getTree();
    
    if (alg) processNode(voxels, voxelOctree, root, 
        NodeIndex(0, 0, 0, maxTreeHeight));
    
    cout << "The time required to complete the operation was: ";
    timer.print(); 
    cout << endl;
    
    return voxels;
}

// ##### processNode() ###############################################
void Voxelization::processNode(Voxels* voxels, IndexOct* tree, 
    OctStruct::_octree& root, NodeIndex node) {
    
    OctStruct::_octree& parts = root->parts;
    
    for (int i = 0; i < OCT_PARTS; i++) {
        NodeIndex nextNode = tree->getChild(node, i);
            
        if (&parts[i] == NULL) return;
        
        NodeStatus ns = parts[i].flag;
        
        if ((parts[i].parts == NULL) && (ns != 0) && (ns != NO_OBJECT))
            if (nextNode.getHeight() == 0) {
                (*voxels)[nextNode.getX()][nextNode.getY()][nextNode.getZ()] 
                    = true;
            } else {
                fill (voxels, tree, nextNode);
            }
        
        if (nextNode.getHeight() > 0) {
            OctStruct::_octree nextRoot = &parts[i];
            processNode(voxels, tree, nextRoot, nextNode);
        }
    }
 
 }

// ##### fill() ######################################################
void Voxelization::fill(Voxels* voxels, IndexOct* tree, NodeIndex node) {
    for (int i = 0; i < OCT_PARTS; i++) {
        NodeIndex nextNode = tree->getChild(node, i);
        
        if (nextNode.getHeight() == 0) {
            (*voxels)[nextNode.getX()][nextNode.getY()][nextNode.getZ()] 
                = true;
        } else {
            fill (voxels, tree, nextNode);
        }
    }
}

// EOF: voxel/voxelization/voxelization.cpp
