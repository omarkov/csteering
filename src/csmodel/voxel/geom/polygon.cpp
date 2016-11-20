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
 * file: voxel/geom/polygon.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <polygon.h>

#include <assert.h>
#include <iostream>
#include <math.h>

using namespace std;

// ##### Polygon() ###################################################
Polygon::Polygon(GeomPoint pA, GeomPoint pB, GeomPoint pC) 
    : m_cornerCount(3), m_pointA(pA), m_pointB(pB), m_pointC(pC), m_pointD(pC) {
    assert (pA.getHeight() == pB.getHeight() 
         && pB.getHeight() == pC.getHeight());
}

// ##### Polygon() ###################################################
Polygon::Polygon(GeomPoint pA, GeomPoint pB, GeomPoint pC, GeomPoint pD) 
    : m_cornerCount(4), m_pointA(pA), m_pointB(pB), m_pointC(pC), m_pointD(pD) {
    assert (Polygon(pA, pB, pC).isInPlane(pD));
}

// ##### getCount() ##################################################
int Polygon::getCount() {
    return m_cornerCount;
}

// ##### getDup() ####################################################
Polygon Polygon::getDup() {
  
    if (getCount() == 3) {
        return Polygon(m_pointA.getDup(), m_pointB.getDup(), m_pointC.getDup());
    }
  
    return Polygon(m_pointA.getDup(), m_pointB.getDup(),
                   m_pointC.getDup(), m_pointD.getDup());
}

// ##### getFootpoint() ##############################################
GeomPoint Polygon::getFootpoint(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    GeomVec vn = getNormVec();
    
    assert (vn.getHeight() == p.getHeight());
    
    Coordinate d = vn * getPoint(0);
    
    return p - (vn * p - d) * vn;
}

// ##### getHeight() ##################################################
Height Polygon::getHeight() {
    return getPoint(0).getHeight();
}

// ##### getNormVec() ################################################
GeomVec Polygon::getNormVec() {
    GeomVec vAB = GeomVec(getPoint(0), getPoint(1));
    GeomVec vAC = GeomVec(getPoint(0), getPoint(2));

    return vAB.vec_prod(vAC).getNormalize();
}

// ##### getPoint() ##################################################
GeomPoint& Polygon::getPoint(int idx) {
    assert (idx < getCount());
  
    switch (idx) {
        case 0: return m_pointA;
        
        case 1: return m_pointB;
        
        case 2: return m_pointC;
  
        case 3: return m_pointD;
  
        default:;
    }
  
    assert (false);
  
    return m_pointA;
}

// ##### isAtBorder() ################################################
bool Polygon::isAtBorder(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    for (int i = 0; i < getCount(); i++) {
        if (Line(getPoint(i), getPoint((i+1) % getCount())).isInStretch(p)) {
            return true;
        }
    }
    
    return false;
}

// ##### isCorner() ##################################################
bool Polygon::isCorner(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    for (int i = 0; i < getCount(); i++) {
        if (p == getPoint(i)) {
            return true;
        }
    }
    
    return false;
}

// ##### isIn() ######################################################
bool Polygon::isIn(GeomPoint p) {
    Polygon _dup = getDup();

    _dup.setHeight(p.getHeight());
    
    assert (p.getHeight() == _dup.getHeight());

    if (_dup.isCorner(p)) {
        return true;
    }
    
    if (!_dup.isInPlane(p)) {
        return false;
    }
  
    if (_dup.isAtBorder(p)) {
        return true;
    }

    #ifdef CCW_CHECK
    
    assert (getCount() == 3);
    
    bool noPolygon = false;
  
    Axis proj = 0;
    GeomPoint pA = _dup.getPoint(0);
    GeomPoint pB = _dup.getPoint(1);
    GeomPoint pC = _dup.getPoint(2);
  
    for (Axis ax = 0; ax < DIMENSIONS; ax++) {
        if (pA[ax] == pB[ax] && pA[ax] == pC[ax]) {
            if (noPolygon) {
                return false;
            } else {
                noPolygon = true;
                proj = ax;
            }
        }
    }
  
    return _dup.isIn(p, proj);
    
    #else
    
    GeomPoint q = _dup.getFootpoint(p);
    Coordinate angle = 0.0;
  
    for (int i= 0; i < getCount(); i++) {
        GeomVec lQA = GeomVec(q, _dup.getPoint(i));
        GeomVec lQB = GeomVec(q, _dup.getPoint((i+1) % getCount()));
        angle = angle + lQA.getAngle(lQB);
    }
    
    return EQUIVAL(angle, 2.0 * M_PI);
    
    #endif
}

// ##### isIn() 
bool Polygon::isIn(GeomPoint q, Axis proj) {
    assert (GeomVec(q, getFootpoint(q)).isNullVec());

    int polygonCcw = getPoint(0).ccw(getPoint(1), getPoint(2), proj);
  
    for (int i = 0; i < getCount(); i++) {
        int k = (i+1) % getCount();
        int qCcw = q.ccw(getPoint(i), getPoint(k), proj);
        int rotDir = polygonCcw * qCcw;

        if (rotDir == 0) {
            Coordinate l = GeomVec(getPoint(i), getPoint(k)).getSqrLength();
      
            if ( (l <= GeomVec(getPoint(i), q).getSqrLength()) 
              || (l <= GeomVec(q, getPoint(k)).getSqrLength()) ) {
                return false;
            }
        } 
        
        else if (rotDir < 0) {
            return false;
        }
    }
    
    return true;
}

// ##### isInPlane() #################################################
bool Polygon::isInPlane(GeomPoint p) {
    assert (p.getHeight() == getHeight());

    #ifndef ALGORITHM_CHECK_DET
    
    return p == getFootpoint(p);
    
    #else
    
    GeomVec nAB = GeomVec(getPoint(0), getPoint(1));
    GeomVec nAC = GeomVec(getPoint(0), getPoint(2));
    GeomVec lineAP = GeomVec(getPoint(0), p);

    Coordinate det = GeomVec::det(nAB, nAC, lineAP);
    Coordinate areaABC = nAB.vec_prod(nAC).getSqrLength();

    return det * det < DET_MAX * areaABC;
    
    #endif
}

// ##### print() #####################################################
void Polygon::print() {
    cout << "[";
    getPoint(0).print();
    
    for (int i = 1; i < getCount(); i++) {
        cout << ", ";
        getPoint(i).print();
    }
    
    cout << "]";
}

// ##### setHeight() ##################################################
void Polygon::setHeight(Height h) {
    for (int i = 0; i < getCount(); i++) {
        getPoint(i).setHeight(h);
    }
}

// ##### testLine() ##################################################
bool Polygon::testLine(GeomPoint p, GeomPoint &q, 
    bool &intersect, bool &inside) {
    
    assert (p.getHeight() == getHeight());

    Axis ax = RAY_AXIS;
    GeomPoint pA = getPoint(0);
    GeomVec nVec = getNormVec();
    Coordinate divisor = nVec[ax];

    if (EQUIVAL(divisor, 0.0)) {
        inside = false;
        intersect = false;
        return false;
    }
  
    q = p;
    Coordinate t = (GeomVec(pA, p) * nVec) / divisor;
    q.setCoordinate(ax, p[ax] - t);
  
    inside= ((q[ax] - p[ax]) * nVec[ax] >= 0);
    intersect= isIn(q, ax);

    return true;
}

// ##### operator[] ##################################################
GeomVec Polygon::operator[](int idx) {
    return getPoint(idx);
}

// EOF: voxel/geom/polygon.cpp
