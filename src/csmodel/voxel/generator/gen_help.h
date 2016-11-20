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
 * @file voxel/generator/gen_help.h
 * library containing supporting functions for the octal tree generation
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __GEN_HELP_H__
#define __GEN_HELP_H__

#include <cad/cad_model.h>
#include <point.h>
#include <triangle.h>
#include <node_vec.h>
#include <polygon.h>
#include <exception.h>
#include <assert.h>

/**
 * class that contains routines that are needed for the octal tree structure 
 * generation
 * 
 * included are: the conversion function Point -> NodeIndex and the functions
 * for calculating the next consecutive axis for the line approximation based
 * on the Cohem-Sutherland algorithm (slightly adjusted)
 */
class GenHelp {
    
    public:
    
    /**
     * constructor
     * @param maxTreeHeight the maximal height of the octal tree
     * @param minPoint the front lower left point of the enclosing right 
     * parallelepiped
     * @param maxPoint the rear upper right point of the enclosing right 
     * parallelepiped
     * @pre minPoint << maxPoint
     */
    GenHelp(Height maxTreeHeight, Point minPoint, Point maxPoint)
        throw (NotEnoughMemoryException);
    
    /**
     * constructor
     * @param voxelSize the voxel size
     * @param cadModel the CadModel to be processed
     */    
    GenHelp(double voxelSize, CadModel* cadModel)
        throw (NotEnoughMemoryException);	

    /**
     * returns the axis, along which further processing should be done
     * in order to best approximate the ideal line
     * 
     * if the direction of all axes is 0 (the start and the end point
     * of the ideal line are the same), the x-axis is returned, otherwise
     * only those axes are processed, whose direction does not equal zero
     * 
     * @param current current point
     * @param end end point
     * @param line vector of the ideal line
     * @param direction directoion of the ideal line
     * @return the optimal axis
     * @pre current.getHeight() = end.getHeight() = line.getHeight()
     */
    Axis bestAxis(NodeIndex current, NodeIndex end, NodeVec line,
        Direction direction);

    /**
     * returns the Point corresponding to the GeomPoint p
     * @param p GeomPoint
     * @return the corresponding Point of the CadModel
     * @pre p.getHeight() == BASE_NODE_HEIGHT
     */
    Point getCadPoint(GeomPoint p);

    /**
     * returns the GeomPoint corresponding to the Point p
     * @param p the point of the CadModel
     * @return the corresponding GeomPoint
     */
    GeomPoint getGeomPoint(Point p);

    /**
     * returns the node index corresponding to the point p
     * @param p point of the CadModel
     * @return node index
     */
    NodeIndex getNodeIndex(Point p);

    /**
     * returns the polygon corresponding to the triangle
     * 
     * @param t triangle of the CadModel
     * @return Polygon of the geometry model
     * @pre t != NULL
     */
    Polygon getPolygon(Triangle *t);  
    
    /**
     * returns the size of the smallest voxel in the octal tree
     */
    double getVoxelSize();
    
    /**
     * returns the efficiency of the octal tree utilization
     */
    double getEfficiency();
    
    /**
     * returns the maximal tree height of the octal tree
     */
    Height getMaxTreeHeight();
  
    private:
    
    /**
     * returns the coordinate of the CadModel corresponding to the point p of 
     * the axis
     * @param p GeomPoint
     * @param axis coordinate axis
     * @return coordinate in the CadModel
     */
    Coordinate getCadCoord(GeomPoint p, Axis axis);

    /**
     * returns the distance of the test point to the ideal line
     * 
     * the smaller the error, the better is the approximation of the ideal line
     * 
     * @param fromTestpoint path from the test point to the 
     *  end point of the line
     * @param line path from the start point to the end point of the ideal line
     * @return unsigned distance to the ideal line
     * @pre fromTestpoint.getHeight() = line.getHeight()
     */
    Coordinate getError(NodeVec fromTestpoint, NodeVec line);

    /**
     * returns the geometric coordinate corresponding to the point p of the axis
     * @param p point of the CadModel
     * @param axis coordinate axis
     * @return geometric coordinate 
     */
    Coordinate getGeomCoord(Point p, Axis axis);

    /**
     * returns the largest distance between the min and max points 
     * along a coordinate axis
     * 
     * @param min min point
     * @param max max point
     * @pre min << max
     */
    Coordinate getMaxExtent(Point min, Point max);
    
    /**
     * prints basic model and octal tree statistics to the screen
     */
    void printStatistics();
    
    /**
     * reciprocal of the edge length of the octal tree cube on the lowest plane
     */
    Coordinate m_density;

    /**
     * rear lower left point of the octal tree area 
     */
    Point m_min;
    
    /**
     * size of the smallest voxel in the octal tree
     */
    double m_voxelSize;
    
    /**
     * the maximal height of the octal tree
     */
    Height m_maxTreeHeight;
    
    /**
     * the efficiency of the octal tree utilization	
     */
    double m_efficiency;
};

#endif // ! __GEN_HELP_H__
