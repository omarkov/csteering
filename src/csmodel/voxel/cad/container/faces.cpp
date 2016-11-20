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
 * @file: voxel/cad/container/faces.cpp
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <cad/container/faces.h>
#include <cad/objects/plane.h>
#include <cad/objects/point.h>

#include <iostream>

using namespace std;

// ##### equival() ###################################################
/**
 * Is the surface of obj1 and obj2 the same?
 * @param obj1 surface no.1
 * @param obj2 surface no.2
 * @return \em true,  if obj1 and obj2 are of the same data type and have
 *  the same border points aligned in the same order
 *         \em false, otherwise
 * @pre obj1 != NULL and obj2 != NULL
 */
bool equival(Face* obj1, Face* obj2) {
    assert ((obj1 != NULL) && (obj2 != NULL));

    if (obj1->getDataType() != obj2->getDataType()) {
        return false;
    }
  
    switch (obj1->getDataType()) {
        case CadObject::TRIANGLE:
            for (int corner = 0; corner < ((Plane *)obj1)->getCornerCount();
                ++corner) {
                if (! ((*((Plane *)obj1))[corner] 
                    == (*((Plane *)obj2))[corner]) ) {
                    return false;
                }
            }
        break;
  
        default:
            return false;
    }
  
    return true;
}

// ##### Faces() #####################################################
Faces::Faces() {
}

// ##### Faces() 
Faces::Faces(Face* object) {
    add(object);
}

// ##### ~Faces() ####################################################
Faces::~Faces() {
}

// ##### add() #######################################################
void Faces::add(Face* object) {
    assert (object != NULL);
    
    m_objs.insert(m_objs.begin(), object);
}

// ##### clear() #####################################################
void Faces::clear() {
    m_objs.clear();
}

// ##### count() #####################################################
unsigned Faces::count() {
    return m_objs.size();
}

// ##### empty() #####################################################
bool Faces::empty() {
    return m_objs.empty();
}

// ##### exist() #####################################################
bool Faces::exist(Face* object) {
    Iterator it = find(object);
    return exist(it);
}

// ##### exist() #####################################################
bool Faces::exist(Iterator it) {
    return it.m_faceIt != m_objs.end();
}

// ##### find() ######################################################
Faces::Iterator Faces::find(Face* object) {
    Iterator it;
  
    for (it.m_faceIt = m_objs.begin(); it.m_faceIt != m_objs.end(); 
            ++it.m_faceIt) {
        if (equival(*it.m_faceIt, object)) {
            return Iterator(it);
        }
    }
  
    return Iterator(it);
}

// ##### first() #####################################################
void Faces::first() {
    m_currentObject.m_faceIt= m_objs.begin();
}

// ##### getObject() #################################################
Element Faces::getObject() throw (OutsideOfModelException*) {
    if (!hasObject()) { 
        throw new OutsideOfModelException();
    }
    
    return *m_currentObject.m_faceIt;
}

// ##### hasObject() #################################################
bool Faces::hasObject() {
    return exist(m_currentObject);
}

// ##### next() ######################################################
void Faces::next() throw (OutsideOfModelException*) {
    if (!hasObject()) {
        throw new OutsideOfModelException();
    }
    
    ++m_currentObject.m_faceIt;
}

// ##### print() #####################################################
void Faces::print() {
    first();
  
    while (hasObject()) {
        cout << "\t\t";
        getObject()->print();
        next();
    }
}

// EOF: voxel/cad/container/faces.cpp
