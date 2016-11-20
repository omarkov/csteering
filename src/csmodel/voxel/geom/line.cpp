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
 * file: voxel/geom/line.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <line.h>

#include <assert.h>
#include <iostream>

using namespace std;

// ##### Line() ######################################################
Line::Line(GeomPoint pA, GeomPoint pB) 
    : m_pointA(pA), m_pointB(pB) {
    assert (pA.getHeight() == pB.getHeight());
}

// ##### getCount() ##################################################
int Line::getCount() {
    return 2;
}

// ##### getDirVec() #################################################
GeomVec Line::getDirVec() {
    return GeomVec(getPoint(0), getPoint(1));
}

// ##### getDistVec() ################################################
GeomVec Line::getDistVec(GeomPoint p) {
    return GeomVec(p, getFootpoint(p));
}

// ##### getFootpoint() ##############################################
GeomPoint Line::getFootpoint(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    GeomVec vAB = getDirVec();
    GeomVec vAP = GeomVec(getPoint(0), p);
  
    if (vAB.isNullVec()) {
        return getPoint(0);
    }
  
    GeomPoint q = ((vAP * vAB) / vAB.getSqrLength()) * vAB + getPoint(0);

    return q;
}

// ##### getHeight() ##################################################
Height Line::getHeight() {
    return getPoint(0).getHeight();
}

// ##### getPoint() ##################################################
GeomPoint Line::getPoint(int idx) {
    switch (idx) {
        case 0: return m_pointA;
        
        case 1: return m_pointB;
        
        default:;
    }
  
    assert (false);
    
    return NULL_GEOM_VEC;
}

// ##### isAtLine() ##################################################
bool Line::isAtLine(GeomPoint p) {
    assert (p.getHeight() == getHeight());
  
    return p == getFootpoint(p);
}

// ##### isEndpoint() ################################################
bool Line::isEndpoint(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    for (int i = 0; i < getCount(); i++) {
        if (p == getPoint(i)) {
            return true;
        }
    }
    
    return false;
}

// ##### isInStretch() ###############################################
bool Line::isInStretch(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    if (isEndpoint(p)) {
        return true;
    }

    if (!isAtLine(p)) {
        return false;
    }

    Coordinate lAP = GeomVec(getPoint(0), p).getSqrLength();
    Coordinate lBP = GeomVec(getPoint(1), p).getSqrLength();
    Coordinate lAB = GeomVec(getPoint(0), getPoint(1)).getSqrLength();

    return (lAP < lAB) && (lBP < lAB);
}

// ##### print() #####################################################
void Line::print() {
    cout << "[";
    
    getPoint(0).print();
    
    for (int i = 1; i < getCount(); i++) {
        cout << ", ";
        getPoint(i).print();
    }
    
    cout << "]";
}

// ##### setHeight() ##################################################
void Line::setHeight(Height h) {
    for (int i = 0; i < getCount(); i++) {
        getPoint(i).setHeight(h);
    }
}

// ##### operator[] ##################################################
GeomPoint Line::operator[](int idx) {
    return getPoint(idx);
}

// EOF: voxel/geom/line.cpp
