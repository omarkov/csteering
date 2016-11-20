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
 * @file voxel/generator/scan_line.h
 * draws line using node indeces (based on Bresenham algorithm)
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __SCAN_LINE_H__
#define __SCAN_LINE_H__

#include <node_vec.h>

/**
 * class for calculating the following point of the line using node indices
 */
class ScanLine {
    
    public:
  
    /**
     * constructor
     * @param start start point of the line
     * @param end end point of the line
     * @pre start.getHeight() = end.getHeight()
     */
    ScanLine(NodeIndex start, NodeIndex end);

    /**
     * returns the current point of the line
     * @return node index of the current point
     */
    NodeIndex getCurrent();

    /**
     * checks whether the end point of the line was reached
     * @return ('internal current point') != end point 
     */
    bool hasNext();

    /**
     * proceeds to the next point of the line
     */
    void next();

    private:
    
    #ifndef COMB_SCAN_LINE
  
    /**
     * returns the absolute error in respect to the ideal line while moving 
     * to the next point along the axis testAx
     * @param testAx axis
     * @return absolute error
     */
    AxIndex getError(Axis testAx);

    #else

    /**
     * returns the absolute error in respect to the ideal line while moving 
     * to the next point along the axes testAxComb
     * @param testAxComb axes
     * @return Absoluter error
     */
    AxIndex getErrorComb(AxComb testAxComb);
    
    #endif

    #ifndef COMB_SCAN_LINE
  
    /**
     * returns the error vector if moving further along the axis testAx
     * @param testAx axis 
     * @return error vector
     */
    NodeIndex getErrorVec(Axis testAx);
    
    #else
  
    /**
     * returns the error vector if moving further along the axes testAxComb
     * @param testAxComb axes
     * @return error vector
     */
    NodeIndex getErrorCombVec(AxComb testAxComb);

    #endif

    #ifndef COMB_SCAN_LINE
    
    /**
     * neighbouring point in the direction of the ax axis
     * @param ax axis
     */
    NodeIndex getNext(Axis ax);
    
    #else
    
    /**
     * neighbouring point in the direction of the axComb axes
     * @param axComb axes
     */
    NodeIndex getNextComb(AxComb axComb);
    
    #endif

    /**
     * returns the maximal component of the error vector testError
     * @param testError error vector
     * @return maximal component
     */
    AxIndex maxError(NodeIndex testError);

    /**
     * returns the minimal component of the error vector testError
     * @param testError error vector
     * @return minimal component
     */
    AxIndex minError(NodeIndex testError);

    #ifndef COMB_SCAN_LINE
  
    /**
     * returns the internal error register
     */
    void refreshError(Axis stepAx);
    
    #else
  
    /**
     * returns the internal error register
     */
    void refreshErrorComb(AxComb stepAxComb);
    
    #endif

    /**
     * current scan position
     */
    NodeIndex m_current; 
  
    /**
     * direction of the line
     */
    NodeIndex m_dir;
  
    /**
     * end point of the line
     */
    NodeIndex m_end;
  
    /**
     * error vector
     */
    NodeIndex m_error;
};

#endif // ! __SCAN_LINE_H__
