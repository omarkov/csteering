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
 * @file voxel/geom/surface.h
 * surface that is defined by its geometric points
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __SURFACE_H__
#define __SURFACE_H__

#include <geom_point.h>

/**
 * surface that is defined by its geometric points
 */
class Surface {
    public:
    
    /**
     * returns the virtual height of the surface in the octal tree
     * @return height of the surface in the octal tree
     */
    virtual Height getHeight() = 0;  

    /**
     * checks whether p is on this surface
     * @param p point
     * @return p is on the surface or not
     */
    virtual bool isIn(GeomPoint p) = 0;

    /**
     * sets the virtual height of the surface in the octal tree
     * @param h height
     */
    virtual void setHeight(Height h) = 0;  

    /**
     * returns whether testLine() was successful
     * @param p start point
     * @param q the intersection point if the test line crosses the surface
     * @param intersect the test line that is parallel to the RAY_AXIS 
     *  and passes through p, crosses the surface
     * @param inside true if the orientation vector of the surface points
     *  in the direction p when in q
     * @return true, if the test line is not on the surface plane
     */
    virtual bool testLine(GeomPoint p, GeomPoint &q, bool &intersect, 
        bool &inside)= 0;

}; // class Surface

#endif // ! __SURFACE_H__
