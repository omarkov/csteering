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
 * file: voxel/cad/cad_model.cpp
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <cad/cad_model.h>

#include <iostream>

using namespace std;

// ##### CadModel() ##################################################
CadModel::CadModel()
    : Faces(),
    m_maxPoint(MINIMUM_POSSIBLE_POINT), m_minPoint(MAXIMUM_POSSIBLE_POINT),
    m_realMaxPoint(MINIMUM_POSSIBLE_POINT), m_realMinPoint(MAXIMUM_POSSIBLE_POINT) {
}

// ##### add() #######################################################
void CadModel::add(Element object, Color color) {
    assert (object != NULL);

    Faces::add(object);
    m_objColor = color;
    m_maxPoint = object->max(m_maxPoint);
    m_minPoint = object->min(m_minPoint);
    m_realMaxPoint = m_maxPoint;;
    m_realMinPoint = m_minPoint;
}

// ##### getMaxPoint() ###############################################
Point CadModel::getMaxPoint() {
    return m_maxPoint;
}

// ##### getMinPoint() ###############################################
Point CadModel::getMinPoint() {
    return m_minPoint;
}

// ##### getRealMaxPoint() ###########################################
Point CadModel::getRealMaxPoint() {
    return m_realMaxPoint;
}

// ##### getRealMinPoint() ###########################################
Point CadModel::getRealMinPoint() {
    return m_realMinPoint;
}

// ##### setMaxPoint() ###############################################
void CadModel::setMaxPoint(Point maxPoint){
	for (int i = 0; i < DIMENSIONS; i++) {
		if (maxPoint[i] < m_realMaxPoint[i])
		    throw ModelBoundaryExc();
	} 
	
	m_maxPoint = maxPoint;
}

// ##### setMinPoint() ###############################################
void CadModel::setMinPoint(Point minPoint){
	for (int i = 0; i < DIMENSIONS; i++) {
		if (minPoint[i] > m_realMinPoint[i])
		    throw ModelBoundaryExc();
	} 
	
	m_minPoint = minPoint;
}

// ##### getObjColor() ###############################################
Color CadModel::getObjColor() throw (OutsideOfModelException*) {
    return m_objColor;
}

// ##### print() #####################################################
void CadModel::print() {
    cout << "CadModel: ";
    
    if (empty()) {
        cout << " is empty." << endl;
        return;
    }
	
	cout << endl << "Min point: " << m_minPoint.getX() << ", " 
	    << m_minPoint.getY() << ", " << m_minPoint.getZ() << endl;
	cout << "Max point: " << m_maxPoint.getX() << ", "
	    << m_maxPoint.getY() << ", " << m_maxPoint.getZ() << endl;
	
    cout << endl;
    //Faces::print();
}

// EOF: voxel/cad/cad_model.cpp
