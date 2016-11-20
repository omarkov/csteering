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
 * file: voxel/reader/reader.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <reader/reader.h>
#include <triangle.h>
#include <octree/oct_struct.h>

using namespace std;

// ##### Reader() ####################################################
Reader::Reader(RawReader::TriangleCoordType* trs){
    m_cadModel = new CadModel();
    m_countTriangles = 0;
    m_countVertices = 0;
    m_color = 1;
    add(trs);   
}

// ##### ~Reader() ###################################################
Reader::~Reader(){
}

// ##### add() #######################################################
void Reader::add(RawReader::TriangleCoordType *triangles) {
    assert (m_cadModel != NULL);
    assert (triangles != NULL);

    RawReader::TriangleCoordType::const_iterator it = triangles->begin();

    while (it != triangles->end()) {

        RawReader::TriangleCoord* tr = *it;
        
        RawReader::Coord* tr_a = tr->a;
        RawReader::Coord* tr_b = tr->b;
        RawReader::Coord* tr_c = tr->c;
        
        Triangle* cadTr = new Triangle(Point(tr_a->x, tr_a->y, tr_a->z), 
            Point(tr_b->x, tr_b->y, tr_b->z),   
            Point(tr_c->x, tr_c->y, tr_c->z));
        
        m_cadModel->add(cadTr, m_color);
        
        m_countVertices += 3;
        m_countTriangles++;
        it++;
    }
}

// ##### getCadModel() ###############################################
CadModel* Reader::getCadModel() {
  assert (m_cadModel != NULL);

  cout << "#vertices = " << m_countVertices << endl;
  cout << "#triangles = " << m_countTriangles << endl;
  
  m_cadModel->print();

  return m_cadModel;
}

// EOF: voxel/reader/reader.cpp
