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
 * @file voxel/cad/objects/cad_object.h
 * defines general methods for defining a CAD object
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __CAD_OBJECT_H__
#define __CAD_OBJECT_H__

class Point;

/**
 * abstract class for all CAD objects,
 * can be a surface (face) or a point
 */
class CadObject {
    
    public:     
    
    /**
     * possible types of objects
     */
    enum DataType {TRIANGLE,        ///< triangle
                   POINT            ///< point
    };
 
    /**
     * returns the type of the CAD object
     * @return the type of the CAD object
     */
    virtual const DataType getDataType() = 0;

    /**
     * returns how many dimensions the CAD object has
     * @return number of dimensions of the CAD object
     */
    virtual int getDimension() = 0;

    /**
     * returns the the maximal point of all the coordinates of the CAD object
     * 
     * @return the upper right point of the right parallelepiped, 
     *  enlosing the CAD object
     */
    virtual Point getMaxPoint() = 0;

    /**
     * returns the minimal point of all the coordinates of the CAD object
     * 
     * @return the lower left point of the right parallelepiped, 
     *  enlosing the CAD object
     */
    virtual Point getMinPoint() = 0;

    /**
     * is the object plane?
     * @return false
     */
    virtual bool isPlane();

    /**
     * returns the point that has the maximal coordinates of the internal
     * maxPoint and the point p
     * 
     * @param p point of comparison
     * @return \f$ \sum_{i=0}^{\dim-1} \max \{ \mbox{getMinPoint()[i]}, p[i] \}
     *                                 \ast \vec e_i \f$ 
     */
    Point max(Point p);

    /**
     * returns the point that has the minimal coordinates of the internal
     * minPoint and the point p
     * 
     * @param p point of comparison
     * @return \f$ \sum_{i=0}^{\dim-1} \min \{ \mbox{getMinPoint()[i]}, p[i] \}
     *                                 \ast \vec e_i \f$ 
     */
    Point min(Point p);

    /**
     * prints the geometry data of the CAD object to the console
     */
    virtual void print() = 0;

};

#endif // ! __CAD_OBJECT_H__
