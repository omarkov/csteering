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
 * file: voxel/octree/oct_gen.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <generator/oct_gen.h>
#include <generator/scan_line.h>
#include <octree/fill_oct.h>
#include <triangle.h>
#include <global.h>
#include <timer.h>

#include <cmath>
#include <iostream>
#include <iomanip>

#include <CommonServer.h>
#include <FANClasses.h>

using namespace std;

// ##### OctGen() ####################################################
OctGen::OctGen(CadModel* model) : m_cadModel(model) {
    assert (model != NULL);
    m_maxTreeHeight = 0;
    m_genHelp = NULL;
    m_octree = NULL;
    setChanged(true);
}

// ##### OctGen() ####################################################
OctGen::OctGen(CadModel* model, Height height) : m_cadModel(model) {
	
	if (height < 0) throw new BadHeightException();
	
    assert (model != NULL);
    m_maxTreeHeight = height;
    m_voxelSize = 0.0;
    m_genHelp = NULL;
    m_octree = NULL;
    setChanged(true);
}

// ##### OctGen() ####################################################
OctGen::OctGen(CadModel* model, double voxelSize) : m_cadModel(model) {
    assert (model != NULL);
    m_voxelSize = voxelSize;
    m_maxTreeHeight = 0;
    m_genHelp = NULL;
    m_octree = NULL;
    setChanged(true);
}

// ##### ~OctGen() ###################################################
OctGen::~OctGen() {
    delete m_genHelp;
    delete m_octree;
}

// ##### add() #######################################################
void OctGen::add(NodeIndex p, Color color)
        throw (NotEnoughMemoryException*){
    m_octree->add(p, color);
}

// ##### addLine() ###################################################
void OctGen::addLine(NodeIndex start, NodeIndex end, Color color)
        throw (NotEnoughMemoryException*) { 
    assert (start.getHeight() == end.getHeight());
    assert (m_octree->isIn(start));
    assert (m_octree->isIn(end));

    ScanLine l = ScanLine(start, end);
    add(l.getCurrent(), color);
    
    while (l.hasNext()) {
        l.next();
        add(l.getCurrent(), color);
    }
}

// ##### addObject() #################################################
void OctGen::addObject(CadObject* object, Color color)
        throw (NotEnoughMemoryException*) {
    assert (object != NULL);
    
    switch (object->getDataType()) {
        
        case CadObject::POINT:
            m_octree->add(m_genHelp->getNodeIndex(*((Point *)object)), color);
            break;
        
        case CadObject::TRIANGLE:
            
            #ifdef ALGORITHM_ISIN
            
            m_octree->add(new Polygon(m_genHelp->getPolygon((Triangle *)object)), 
                color);
            
            #else
                
            addTriangle(m_genHelp->getNodeIndex((*((Triangle *)object))[0]),
                        m_genHelp->getNodeIndex((*((Triangle *)object))[1]),
                        m_genHelp->getNodeIndex((*((Triangle *)object))[2]),
                        color);
            
            #endif
            
            break;    
    }
}

// ##### addTriangle() ###############################################
void OctGen::addTriangle(NodeIndex pA, NodeIndex pB, NodeIndex pC,
    Color color)
        throw (NotEnoughMemoryException*) {
    assert (pA.getHeight() == pB.getHeight() && pB.getHeight() 
        == pC.getHeight());
    assert (m_octree->isIn(pA));
    assert (m_octree->isIn(pB));
    assert (m_octree->isIn(pC));

    ScanLine l1 = ScanLine(pA, pB);
    ScanLine l2 = ScanLine(pA, pC);
    addLine(l1.getCurrent(), l2.getCurrent(), color);
    
    while (l1.hasNext() || l2.hasNext()) {
        
        if (l1.hasNext()) {
            l1.next();
        }
    
        if (l2.hasNext()) {
            l2.next();
        }
    
        addLine(l1.getCurrent(), l2.getCurrent(), color);
    }
}

// ##### checkModel() ################################################
void OctGen::checkModel() throw (BadModelException*) {
    
    if (m_cadModel == NULL) {
        throw new BadModelException();
    }
  
    if (m_cadModel->empty()) {
        throw new BadModelException();
    }
  
    if (m_cadModel->getMinPoint() == m_cadModel->getMaxPoint()) {
        throw new BadModelException();
    }
}

#ifdef CLASSIC_MODE

// ##### generateClassic() ###########################################
void OctGen::generateClassic(NodeIndex idx) {
    assert (m_cadModel != NULL);

    bool isAtBorder;
    Color color = testRayColor(idx, isAtBorder);
  
    if ((!isAtBorder) || (idx.getHeight() == BASE_NODE_HEIGHT)) {
        m_octree->add(idx, color);
        return;
    }
  
    for (PartType part = 0; part < OCT_PARTS; part++) {
        generateClassic(m_octree->getChild(idx, part));
    }
}

#endif

// ##### getCadModel() ###############################################
CadModel* OctGen::getCadModel() {
    assert (m_cadModel != NULL);

    return m_cadModel;
}

#ifndef CLASSIC_MODE

// ##### getColor() ##################################################
Color OctGen::getColor(NodeIndex p) {
    
    #ifdef RAY_METHOD
    
    return testRayColor(p);
    
    #else
    
    return nextPointColor(p);
    
    #endif
}

#endif

// ##### getGenHelp() ################################################
GenHelp* OctGen::getGenHelp() {
    assert (m_genHelp != NULL);

    return m_genHelp;
}

// ##### getGenTree() ################################################
IndexOct* OctGen::getGenTree() {
    assert (m_octree != NULL);

    return m_octree;
}

// ##### genOctree() #################################################
IndexOct* OctGen::genOctree(Height maxTreeHeight)
        throw (NotEnoughMemoryException*, BadModelException*,
            BadHeightException*) {
        	
    if (maxTreeHeight < 0) throw new BadHeightException();    	
    
    m_maxTreeHeight = maxTreeHeight;
    m_genHelp = new GenHelp(m_maxTreeHeight, m_cadModel->getMinPoint(),
        m_cadModel->getMaxPoint());
    m_voxelSize = m_genHelp->getVoxelSize();
    return genOctree();
}

// ##### genOctree() #################################################
IndexOct* OctGen::genOctree(double voxelSize)
        throw (NotEnoughMemoryException*, BadModelException*) {
    m_voxelSize = voxelSize;
    m_genHelp = new GenHelp(voxelSize, m_cadModel);
    m_maxTreeHeight = m_genHelp->getMaxTreeHeight();
    return genOctree();
}

// ##### genOctree() #################################################
IndexOct* OctGen::genOctree()
        throw (NotEnoughMemoryException*, BadModelException*) {
        	
    unsigned borderNodes, sumNodes, leaves, innerNodes, normcells;

    checkModel();
    
    Timer timer;
    
    m_octree = new IndexOct(m_maxTreeHeight);

    timer.reset();
  
    #ifdef CLASSIC_MODE
    
    cout << "Octree generation in progress (classic mode)..." << endl; 
    
    NodeIndex idx = NodeIndex(0, 0, 0, m_octree->getMaxTreeHeight());
    generateClassic(idx);
    
    cout << endl;
    cout << "The time required to complete the operation was: ";
    timer.print(); 
    cout << endl;
    
    #else // !CLASSIC_MODE
    
    cout << "The surfaces are being inserted..." << endl;
    
    unsigned maxCount = m_cadModel->count() - 1;
    
    cout << " [Nr] " <<  maxCount + 1 << endl;
    
    int modelSize = m_cadModel->count();
    int count = 0;

    m_cadModel->first();

    cout << "Progress: " << endl;

    int steps = modelSize / 5;
    
    while (m_cadModel->hasObject()) {

        Element obj = m_cadModel->getObject();
        
        assert (obj != NULL);

	if(visConn != NULL)
	{
            if(!(count % steps))
            {
	        char *status = NULL;
	        asprintf(&status, "%d",  (int)((float)(count) / (float)modelSize * 50.0f));
                visConn->rpc("vis::setServerStatus", 2, "Voxelizing model", status);
	        free(status);
	    }
	}else
	{
            cout << setiosflags(ios::right) << setiosflags(ios::fixed) 
                << setprecision(2) << setw(5) << (static_cast<double>(count) / modelSize * 100) << "%";
    	
            cout << "\b\b\b\b\b\b\b";
   	    cout << flush;
	}

       addObject(obj, m_cadModel->getObjColor());
        m_cadModel->next();
        count++;
    }

    cout << "100.00%";
  
    cout << endl;
  
    cout << "The time required to complete the operation was: ";
    timer.print(); 
    cout << endl;

    #ifdef FILL_SOLIDS
    
    m_octree->stat(m_octree->getMaxTreeHeight(), sumNodes, leaves, innerNodes, 
        borderNodes, normcells);

    cout << "#nodes = " << formatLarge(sumNodes) 
         << ", #leaves = " << formatLarge(leaves)
         << ", #inner nodes = " << formatLarge(innerNodes) << endl;
    cout << "#border nodes = " << formatLarge(borderNodes) 
         << ", #normcells = " << formatLarge(normcells) << endl;
    cout << "The solids are being filled...";
    timer.reset();

    FillOct(*m_octree).fill(this);
  
    cout << "The time required to complete the operation was: ";
    timer.print(); 
    cout << endl;
  
    #endif // FILL_SOLIDS

    #endif // !CLASSIC_MODE
    
    m_octree->stat(m_octree->getMaxTreeHeight(), sumNodes, leaves, innerNodes, 
        borderNodes, normcells);

    cout << "#nodes = " << formatLarge(sumNodes) 
         << ", #leaves = " << formatLarge(leaves)
         << ", #inner nodes = " << formatLarge(innerNodes) << endl;
    cout << "#border nodes = " << formatLarge(borderNodes) 
         << ", #normcells = " << formatLarge(normcells) << endl;
    timer.reset();
    cout << "Flushing..." << endl;

    m_octree->flush();

    cout << "The time required to complete the operation was: ";
    timer.print(); 
    cout << endl;
  
    m_octree->stat(m_octree->getMaxTreeHeight(), sumNodes, leaves, innerNodes, 
        borderNodes, normcells);

    cout << "#nodes = " << formatLarge(sumNodes) 
         << ", #leaves = " << formatLarge(leaves)
         << ", #inner nodes = " << formatLarge(innerNodes) << endl;
    cout << "#border nodes = " << formatLarge(borderNodes) 
         << ", #normcells = " << formatLarge(normcells) << endl;
       
    Point minPoint = m_cadModel->getMinPoint();
    Point maxPoint = m_cadModel->getMaxPoint();
  
    m_octree->setMinPoint(minPoint);
    m_octree->setMaxPoint(maxPoint);
  
    m_octree->setMinGeomPoint(m_genHelp->getGeomPoint(minPoint));
    m_octree->setMaxGeomPoint(m_genHelp->getGeomPoint(maxPoint));
       
    return m_octree;
}

// ##### getOctree() #################################################
IndexOct* OctGen::getOctree(Height maxTreeHeight)
    throw (NotEnoughMemoryException*, BadModelException*,
        BadHeightException*) {
    	
    if (maxTreeHeight < 0) throw new BadHeightException();	
    
    if ((m_maxTreeHeight != maxTreeHeight)
        || (m_octree == NULL) || m_changed){
        m_octree = genOctree(maxTreeHeight);
        m_maxTreeHeight = maxTreeHeight;
    }
    
    setChanged(false);
        
    return m_octree;
}

// ##### getOctree() #################################################
IndexOct* OctGen::getOctree(double voxelSize)
    throw (NotEnoughMemoryException*, BadModelException*) {
    
    if (((m_voxelSize - voxelSize) > EPSILON)
        || (m_octree == NULL) || m_changed){
        m_octree = genOctree(voxelSize);
        m_voxelSize = voxelSize;
    }
    
    setChanged(false);
        
    return m_octree;
}

// ##### getOctree() #################################################
IndexOct* OctGen::getOctree()
    throw (NotEnoughMemoryException*, BadModelException*) {
    
    if ((m_octree == NULL) || m_changed) {
    	
    	if (m_maxTreeHeight != 0){
        	m_octree = genOctree(m_maxTreeHeight);
    	} else {
    		m_octree = genOctree(m_voxelSize);
    	}
        	
    }
    
    setChanged(false);
    
    return m_octree;
}

// ##### getChanged() ################################################
bool OctGen::getChanged() {
    return m_changed;
};
  
// ##### setChanged() ################################################  
void OctGen::setChanged(bool changed){
    m_changed = changed;
};

// ##### nextPointColor() ############################################
Color OctGen::nextPointColor(NodeIndex p) {
    assert (p.getHeight() == BASE_NODE_HEIGHT);
    assert (m_cadModel != NULL);
    assert (m_octree != NULL);
    assert (m_genHelp != NULL);

    Color color;

    m_cadModel->first();
    
    while (!m_cadModel->getObject()->isPlane() && m_cadModel->hasObject()) {
        m_cadModel->next();
    }

    Coordinate sqrDist = MAX_AX_INDEX * MAX_AX_INDEX * MAX_AX_INDEX;
    GeomPoint footpoint = NULL_GEOM_VEC;
    Polygon polygon = Polygon(footpoint, footpoint, footpoint);
    color = NO_OBJECT;
  
    while (m_cadModel->hasObject()) {
        CadObject* object = m_cadModel->getObject();
    
        switch (object->getDataType()) {
            case CadObject::TRIANGLE:
                polygon = m_genHelp->getPolygon((Triangle *)object);
            break;
            
            default:;
        }

        bool haveMorePolygons = true;
        
        while (haveMorePolygons) {
            footpoint = polygon.getFootpoint(p);
            
            if (!polygon.isIn(footpoint)) {
                return false;
            }
      
            GeomVec rayVec = GeomVec(GeomPoint(p), footpoint);
            Coordinate angle = polygon.getNormVec() * rayVec;
      
            if (EQUIVAL(rayVec.getSqrLength(), sqrDist)) {
            } 
            else if (rayVec.getSqrLength() < sqrDist) {
                sqrDist = rayVec.getSqrLength();
                color = (angle <= 0.0) ? NO_OBJECT : m_cadModel->getObjColor();
            }
      
            haveMorePolygons = false;
        }
    
        m_cadModel->next();
    }
    
    return color;
}

// ##### testRayColor() ##############################################

#ifdef CLASSIC_MODE

Color OctGen::testRayColor(NodeIndex p, bool &isAtBorder) {
    isAtBorder = false;

#else

Color OctGen::testRayColor(NodeIndex p) {

#endif
    
    assert (cadModel != NULL);
    assert (octree != NULL);
    assert (genHelp != NULL);

    Axis rayAxis = RAY_AXIS;
    AxDirection rayDir = RAY_DIR;

    assert (rayAxis < DIMENSIONS);
    assert (rayDir == BACKWARD || rayDir == FORWARD);

    Color color = NO_OBJECT;
    bool intersect = false;
    bool inside = false;

    m_cadModel->first();
    GeomPoint idxPoint = GeomPoint(p);
    Coordinate dist = (rayDir == FORWARD ? MAX_AX_INDEX : -MAX_AX_INDEX);
    GeomPoint footpoint = NULL_GEOM_VEC;
    Polygon s(0.0, 0.0, 0.0);
    color = NO_OBJECT;
    
    while (m_cadModel->hasObject()) {
        CadObject* object = m_cadModel->getObject();
        
        switch (object->getDataType()) {
            case CadObject::TRIANGLE:
                s = m_genHelp->getPolygon((Triangle *)object);
                break;
                
            default:;
    
        }

        #ifdef CLASSIC_MODE
    
        s.setHeight(idxPoint.getHeight());
        
        #else
        
        idxPoint.setHeight(BASE_NODE_HEIGHT);
        
        #endif
        
        assert (s.getHeight() == idxPoint.getHeight());

        Coordinate pCoord= idxPoint[rayAxis];
        bool haveMorePolygons = true;

        // while (haveMorePolygons) {
            assert (s.getHeight() == idxPoint.getHeight());

            #ifdef CLASSIC_MODE
            
            if (s.isIn(idxPoint)) {
                isAtBorder = true;
                inside = true;
                return m_cadModel->getObjColor();
            }
            
            #endif
            
            if (s.testLine(idxPoint, footpoint, intersect, inside)) {
                Coordinate coord = footpoint[rayAxis];
                
                if (intersect) {
                
                    if ( (rayDir == FORWARD) ? coord < dist && coord > pCoord
                               : coord > dist && coord < pCoord ) {
                        dist = coord;
                        color = (inside ? m_cadModel->getObjColor() : NO_OBJECT);
                    }
                }
            }
      
       /*     if (t2 != NULL) {
                s = t2;
            
                #ifdef CLASSIC_MODE
        
                s->setHeight(idxPoint.getHeight());
            
                #endif
            
                t2 = NULL;
            
            } else {
                haveMorePolygons = false;
            }
        }*/

        m_cadModel->next();
    }
    
    return color;
}

// EOF: voxel/generator/oct_gen.cpp
