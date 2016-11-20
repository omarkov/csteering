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
 * file: voxel/generator/scan_line.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <generator/scan_line.h>

#include <assert.h>

// ##### ScanLine() ##################################################
ScanLine::ScanLine(NodeIndex start, NodeIndex end) : m_current(start), 
        m_dir(NodeVec(start, end)), m_end(end), m_error(NULL_NODE_VEC) {
    assert (start.getHeight() == end.getHeight());
}

// ##### getCurrent() ################################################
NodeIndex ScanLine::getCurrent() {
    return m_current;
}

#ifndef COMB_SCAN_LINE

// ##### getError() ##################################################
AxIndex ScanLine::getError(Axis testAx) {
    NodeIndex testError = getErrorVec(testAx);
    return maxError(testError);
}

#endif

#ifdef COMB_SCAN_LINE

// ##### getErrorComb() ##############################################
AxIndex ScanLine::getErrorComb(AxComb testAxComb) {
    NodeIndex testError = getErrorCombVec(testAxComb);
    return maxError(testError);
}

#endif

#ifndef COMB_SCAN_LINE

// ##### getErrorVec() ###############################################
NodeIndex ScanLine::getErrorVec(Axis testAx) {
    NodeIndex retVal = m_error;
    AxIndex addErr = 1;

    for (Axis ax = 0; ax < DIMENSIONS; ax++) {
        if ((ax != testAx) && (m_dir[ax] != 0)) {
            addErr *= (m_dir[ax] < 0 ? -m_dir[ax] : m_dir[ax]);
        }
    }

    retVal.setCoordinate(testAx, retVal[testAx] + addErr);
    return retVal;
}

#endif

#ifdef COMB_SCAN_LINE

// ##### getErrorCombVec() ###########################################
NodeIndex ScanLine::getErrorCombVec(AxComb testAxComb) {
    NodeIndex retVal = m_error;
    AxIndex addErr = 1;

    for (Axis testAx = 0; testAx < DIMENSIONS; testAx++) {
        if ((( (1 << testAx) & testAxComb ) != 0) && (m_dir[testAx] != 0)) {
            for (Axis ax = 0; ax < DIMENSIONS; ax++) {
                if ((ax != testAx) && (m_dir[ax] != 0)) {
                    addErr *= (m_dir[ax] < 0 ? -m_dir[ax] : m_dir[ax]);
                }
            }
            
            retVal.setCoordinate(testAx, retVal[testAx] + addErr);
        }
    }

    return retVal;
}

#endif

#ifndef COMB_SCAN_LINE

// ##### getNext() ###################################################
NodeIndex ScanLine::getNext(Axis ax) {
    assert (m_dir[ax] != 0);
    NodeIndex retVal = m_current;
    retVal.setCoordinate(ax, retVal[ax] + SIGN(m_dir[ax]));
    return retVal;
}

#endif

#ifdef COMB_SCAN_LINE

// ##### getNextComb() ###############################################
NodeIndex ScanLine::getNextComb(AxComb axComb) {
    assert (axComb > 0 && axComb <= (1 << DIMENSIONS));
    NodeIndex retVal = m_current;

    for (Axis ax = 0; ax < DIMENSIONS; ax++) {
        if (( (1 << ax) &  axComb ) != 0) {
            retVal.setCoordinate(ax, retVal[ax] + SIGN(m_dir[ax]));
        }
    }

    return retVal;
}

#endif

// ##### hasNext() ###################################################
bool ScanLine::hasNext() {
    return !(m_current == m_end);
}

// ##### maxError() ##################################################
AxIndex ScanLine::maxError(NodeIndex testError) {
    AxIndex max = testError[0];
  
    for (Axis ax = 1; ax < DIMENSIONS; ax++) {
        if (max < testError[ax] && m_dir[ax] != 0) {
            max = testError[ax];
        }
    }
  
    return max;
}

// ##### minError() ##################################################
AxIndex ScanLine::minError(NodeIndex testError) {
    AxIndex min = testError[0];
  
    for (Axis ax = 1; ax < DIMENSIONS; ax++) {
        if (min > testError[ax] && m_dir[ax] != 0) {
            min = testError[ax];
        }
    }
    
    return min;
}

// ##### next() ######################################################
void ScanLine::next() {

#ifndef COMB_SCAN_LINE

    Axis nextAx = 0;
  
    while ((m_dir[nextAx] == 0) && (nextAx < DIMENSIONS)) {
        nextAx++;
    }
    
    assert (nextAx < DIMENSIONS);
  
    AxIndex minError = getError(nextAx);
    
    for (Axis ax = nextAx + 1; ax < DIMENSIONS; ax++) {
        if (m_dir[ax] != 0) {
            AxIndex testErr= getError(ax);
            
            if (testErr < minError) {
                nextAx = ax;
                minError = testErr;
            }
        }
    }
  
    refreshError(nextAx);
    m_current = getNext(nextAx);

    #else
    
    AxComb nextAxComb = 1;
  
    while ( (getNextComb(nextAxComb) == m_current) 
         && (nextAxComb <= (1 << DIMENSIONS)) ) {
            
        nextAxComb++;
    }
    
    assert (nextAxComb <= (1 << DIMENSIONS));
  
    AxIndex minError = getErrorComb(nextAxComb);
  
    for (AxComb axComb = nextAxComb + 1; axComb < (1 << DIMENSIONS); axComb++) {
        if (!(getNextComb(axComb) == m_current)) {
            AxIndex testErr = getErrorComb(axComb);
            
            if (testErr < minError) {
                nextAxComb= axComb;
                minError = testErr;
            }
        }
    }
    
    refreshErrorComb(nextAxComb);
    m_current = getNextComb(nextAxComb);
    
    #endif
}

#ifndef COMB_SCAN_LINE

// ##### refreshError() ##############################################
void ScanLine::refreshError(Axis stepAx) {
    m_error= getErrorVec(stepAx);
    AxIndex min = minError(m_error);
    
    for (Axis ax= 0; ax < DIMENSIONS; ++ax) {
        if (m_dir[ax] != 0) {
            m_error.setCoordinate(ax, m_error[ax] - min);
        }
    }
}

#endif

#ifdef COMB_SCAN_LINE

// ##### refreshErrorComb() ##########################################
void ScanLine::refreshErrorComb(AxComb stepAxComb) {
    m_error = getErrorCombVec(stepAxComb);
    AxIndex min = minError(m_error);
   
    for (Axis ax = 0; ax < DIMENSIONS; ax++) {
        if (m_dir[ax] != 0) {
            m_error.setCoordinate(ax, m_error[ax] - min);
        }
    }
}

#endif

// EOF: voxel/generator/scan_line.cpp
