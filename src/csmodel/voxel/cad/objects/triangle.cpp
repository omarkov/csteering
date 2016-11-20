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
 * file: voxel/cad/objects/triangle.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <cad/objects/triangle.h>

#include <exception.h>

#include <assert.h>
#include <stdlib.h>

// ##### Triangle() ##################################################
Triangle::Triangle(Point pA, Point pB, Point pC)
    : m_pointA(pA), m_pointB(pB), m_pointC(pC) {
}

// ##### getCornerCount() ############################################
int Triangle::getCornerCount() {
    return 3;
}

// ##### getDataType() ###############################################
const CadObject::DataType Triangle::getDataType() {
    return CadObject::TRIANGLE;
}

// ##### operator[] ##################################################
Point Triangle::operator[](int idx) {
    switch (idx) {
        case 0: return m_pointA;
        case 1: return m_pointB;
        case 2: return m_pointC;
    }
  
    assert (false);
    
    return NULL_POINT;
}

// EOF: voxel/cad/objects/triangle.cpp
