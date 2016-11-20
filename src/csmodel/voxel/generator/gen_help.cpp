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
 * file: voxel/generator/gen_help.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <generator/gen_help.h>

#include <iostream>

using namespace std;

// ##### GenHelp() ###################################################
GenHelp::GenHelp(Height maxTreeHeight, Point minPoint, Point maxPoint)
    throw (NotEnoughMemoryException)
        : m_min(minPoint) {
            
    assert (!(minPoint == maxPoint));
    
    m_maxTreeHeight = maxTreeHeight;
    
    double maxExtent = getMaxExtent(minPoint, maxPoint);

    m_density = (Coordinate(1 << maxTreeHeight) - (Coordinate)EPSILON)
        / maxExtent;
        
    m_voxelSize = maxExtent 
        / pow (2.0, static_cast<double>(maxTreeHeight));
    
    //additional check, the result must always equal 1.0 in this mode  
    m_efficiency = maxExtent / (m_voxelSize * pow(2.0, 
        static_cast<double>(m_maxTreeHeight)));
    
    cout << "Height mode" << endl;    
    
    printStatistics();
                   
    assert (!EQUIVAL(m_density, 0.0));
}

// ##### GenHelp() ###################################################
GenHelp::GenHelp(double voxelSize, CadModel* cadModel)
    throw (NotEnoughMemoryException)
        : m_min(cadModel->getMinPoint()) {
            
    m_voxelSize = voxelSize;
            
    Point minPoint = cadModel->getMinPoint();
    Point maxPoint = cadModel->getMaxPoint();
            
    assert (!(minPoint == maxPoint));
    
    double maxExtent = getMaxExtent(minPoint, maxPoint);
    double logRatio = log(maxExtent / voxelSize) / log(2.0);
    
    m_maxTreeHeight = static_cast<Height>(ceil(logRatio));
    
    double newMaxPointDiff = voxelSize * pow(2.0, 
        static_cast<double>(m_maxTreeHeight));
        
    //cadModel->setMaxPoint(Point(minPoint[0] + newMaxPointDiff, 
    //    minPoint[1] + newMaxPointDiff, minPoint[2] + newMaxPointDiff));    
    
    m_density = (Coordinate(1 << m_maxTreeHeight) - (Coordinate)EPSILON)
        / newMaxPointDiff;
        
    m_efficiency = maxExtent / newMaxPointDiff;    
        
    cout << "Voxel size mode" << endl;
    
    printStatistics();
           
    assert (!EQUIVAL(m_density, 0.0));
}

// ##### printsStatistics() ##########################################
void GenHelp::printStatistics() {
	
	cout << "Density: " << m_density << endl;
    
    cout << "Max tree height: " << m_maxTreeHeight << endl;
    
    cout << "Voxel Size: " << m_voxelSize << endl;
    
    cout << "Efficiency: " << m_efficiency << endl;
}

// ##### bestAxis() ##################################################
Axis GenHelp::bestAxis(NodeIndex current, NodeIndex end, NodeVec line,
    Direction direction) {
  
    assert (current.getHeight() == end.getHeight()
         && end.getHeight() == line.getHeight());

    Axis keptAxis = 0;
    NodeIndex testPoint= current;
    Coordinate minError= MAX_AX_INDEX*MAX_AX_INDEX*MAX_AX_INDEX;
    
    for (Axis axis = 0; axis < DIMENSIONS; axis++) {
        if (direction[axis] != 0) {
            testPoint = current;
            testPoint.setCoordinate(axis, testPoint[axis] + direction[axis]);
            Coordinate error = getError(NodeVec(testPoint, end), line);
      
            if (error <= minError) {
                keptAxis = axis;
                minError = error;
            }
        }
    }
  
    return keptAxis;
}

// ##### getCadCoord() ###############################################
Coordinate GenHelp::getCadCoord(GeomPoint p, Axis axis) {
    assert (!EQUIVAL(density, 0.0));

    return m_min[axis] + p[axis] / m_density;
}

// ##### getCadPoint() ###############################################
Point GenHelp::getCadPoint(GeomPoint p) {
    assert (p.getHeight() == BASE_NODE_HEIGHT);

    Point retVal = Point(getCadCoord(p, X_AXIS),
                         getCadCoord(p, Y_AXIS),
                         getCadCoord(p, Z_AXIS));
  
    return retVal;
}

// ##### getError() ##################################################
Coordinate GenHelp::getError(NodeVec fromTestpoint, NodeVec line) {
    assert (fromTestpoint.getHeight() == line.getHeight());

    GeomVec geomLine = GeomPoint(line.getX(), line.getY(), line.getZ());
    GeomVec geomFromTp = GeomPoint(fromTestpoint.getX(), fromTestpoint.getY(),
        fromTestpoint.getZ());
  
    return geomFromTp.vec_prod(geomLine.getNormalize()).getSqrLength();
}

// ##### getGeomCoord() ##############################################
Coordinate GenHelp::getGeomCoord(Point p, Axis axis) {
    return m_density * (p[axis] - m_min[axis]);
}

// ##### getGeomPoint() ##############################################
GeomPoint GenHelp::getGeomPoint(Point p) {
    GeomPoint retVal = GeomPoint(getGeomCoord(p, X_AXIS),
                                 getGeomCoord(p, Y_AXIS),
                                 getGeomCoord(p, Z_AXIS),
                                 BASE_NODE_HEIGHT);

    return retVal;
}

// ##### getNodeIndex() ##############################################
NodeIndex GenHelp::getNodeIndex(Point p) {
    GeomPoint g = getGeomPoint(p);
    
    assert (g.getHeight() == BASE_NODE_HEIGHT);

    NodeIndex retVal = g.getNodeIndex();
  
    assert (retVal.getHeight() == BASE_NODE_HEIGHT);
    
    return retVal;
}

// ##### getMaxExtent() ##############################################
Coordinate GenHelp::getMaxExtent(Point min, Point max) {
    assert (min << max);
  
    Coordinate maxExtent = (max[0] - min[0]);
  
    for (Axis axis = 1; axis < DIMENSIONS; axis++) {
        maxExtent = MAX_VAL(maxExtent, max[axis] - min[axis]);
    }
  
    return maxExtent;
}

// ##### getVoxelSize() ##############################################
double GenHelp::getVoxelSize(){
	return m_voxelSize;
}

// ##### getEfficiency() #############################################
double GenHelp::getEfficiency(){
	return m_efficiency;
}

// ##### getMaxTreeHeight() ##########################################
Height GenHelp::getMaxTreeHeight(){
    return m_maxTreeHeight;
}

// ##### getPolygon() 
Polygon GenHelp::getPolygon(Triangle *t) {
    assert (t != NULL);
    
    Polygon p = Polygon(getGeomPoint((*t)[0]), 
                        getGeomPoint((*t)[1]),
                        getGeomPoint((*t)[2]));
             
    assert (p.getHeight() == BASE_NODE_HEIGHT);
    
    return p;
}

// EOF: voxel/generator/gen_help.cpp
