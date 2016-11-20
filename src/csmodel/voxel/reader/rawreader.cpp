// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Mikhail Prokharau, StuPro A - CS 
// 	<csteering-devel@duck.informatik.uni-stuttgart.de>
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
 * file: voxel/reader/rawreader.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#include <reader/rawreader.h>

using namespace std;

unsigned char *RawReader::m_buffer = NULL;
int RawReader::m_oldSize = 0;

// ##### RawReader() #################################################
RawReader::RawReader(const char* input) {
	m_points = NULL;
	m_triangles = NULL;
	m_triangleCoords = NULL;

	read(input);
}

// ##### RawReader() #################################################
RawReader::RawReader(FACE_TRGL* faces, int num){

	m_points = NULL;
	m_triangles = NULL;

        int num_total = 0;

	for (int i = 0; i < num; i++){
		FACE_TRGL face = faces[i];
		num_total += face.iSizeTriangles;
        }


        int size = num_total * (sizeof(TriangleCoord) + 3 * sizeof(Coord));

        if(RawReader::m_buffer != NULL && size > RawReader::m_oldSize) {
            free(m_buffer);
            RawReader::m_buffer = (unsigned char*)malloc(size);
        }else if(RawReader::m_buffer == NULL)
            RawReader::m_buffer = (unsigned char*)malloc(size);

        RawReader::m_oldSize = size;

        unsigned char* pointer = RawReader::m_buffer;
          
	m_triangleCoords = new TriangleCoordType;
        m_triangleCoords->reserve(num_total);

	cout << "RawReader constructor" << endl;
	
	for (int i = 0; i < num; i++){
		FACE_TRGL face = faces[i];
		CSVERTEX* vertices = face.vertices;
		CSTRIANGLE* triangles = face.triangles;
		
		for (int j = 0; j < face.iSizeTriangles; j++){
			CSTRIANGLE triangle = triangles[j];
			CSVERTEX a = vertices[triangle.a - 1];
			CSVERTEX b = vertices[triangle.b - 1];
			CSVERTEX c = vertices[triangle.c - 1];
			
			TriangleCoord* new_triangle = (TriangleCoord*)(pointer);
        		pointer += sizeof(TriangleCoord);
			// TriangleCoord* new_triangle = new TriangleCoord;
			Coord* new_a = (Coord*)(pointer);
        		pointer += sizeof(Coord);
			// Coord* new_a = new Coord;
			Coord* new_b = (Coord*)(pointer);
        		pointer += sizeof(Coord);
			// Coord* new_b = new Coord;
			Coord* new_c = (Coord*)(pointer);
        		pointer += sizeof(Coord);
			// Coord* new_c = new Coord;
			
			new_a->x = a.x;
			new_a->y = a.y;
			new_a->z = a.z;
			new_b->x = b.x;
			new_b->y = b.y;
			new_b->z = b.z;
			new_c->x = c.x;
			new_c->y = c.y;
			new_c->z = c.z;
			
			new_triangle->a = new_a;
			new_triangle->b = new_b;
			new_triangle->c = new_c;

			m_triangleCoords->push_back((TriangleCoord*)new_triangle);
		}	
	}
}

// ##### ~RawReader() ################################################
RawReader::~RawReader() {	
	cout << "RawReader destructor..." << endl;
	
	if (m_points != NULL) {
		
		cout << "Deleting m_points..." << endl;
		
	    CoordType::iterator it3 = m_points->begin();
	
	    while (it3 != m_points->end()){
	   	    delete *it3;
	   	    it3++;
	    }
		    
	    delete m_points;
	}

	if (m_triangles != NULL) {
		
		cout << "Deleting m_triangles..." << endl;
	
	    TriangleType::iterator it2 = m_triangles->begin();
	
	    while (it2 != m_triangles->end()) {
	  	    delete *it2;
	   	    it2++;
	    }
		    
	    delete m_triangles;
	}
	   
	if (m_triangleCoords != NULL) {  
		
  	    cout << "Deleting m_triangleCoords..." << endl;
	    delete m_triangleCoords;
	}
}

// ##### getTriangles() ##############################################
RawReader::TriangleCoordType* RawReader::getTriangles(void) {
	return m_triangleCoords;
}

// ##### read() ######################################################	
void RawReader::read(const char* input) {
	
	ifstream inp(input);
	
	if (inp == NULL) {
		cerr << "Failed to open file for reading" << endl;
		exit(1);
	}
	
	string line;
	
	try {
		getline(inp, line, '\n');
		
		m_triangleCoords = new TriangleCoordType;
		
		while (inp) {
			//long face = atol (line.c_str());
			
			getline(inp, line, '\n'); 
			long size = atol (line.c_str());
			
			getline(inp, line, '\n');

			string x_str, y_str, z_str;
			
			m_points = new CoordType;
			m_points->reserve(size);
			
			for (int i = 0; i < size; i++){
				
				getline(inp, line, '\n');
				int pos = line.find("\t", 0);
				if (pos == -1) break;
				x_str = line.substr (0, pos);
				
				int pos2 = line.find("\t", pos + 1);
				y_str = line.substr (pos + 1, pos2 - pos - 1);
				
				int pos3 = line.find("\n", pos2 + 1);
				z_str = line.substr (pos2 + 1, pos3 - pos2 - 1);
				
				Coord* next = new Coord;
			
				next->x = atof (x_str.c_str());
				next->y = atof (y_str.c_str());
				next->z = atof (z_str.c_str());
			
				m_points->push_back (next);
			}
			
			getline(inp, line, '\n');	
			
			if (atol(line.c_str()) != 0) throw CoordFormatExc();
			
			string a_str, b_str, c_str;
			
			getline(inp, line, '\n');
			size = atol (line.c_str());
			
			m_triangles = new TriangleType;
			m_triangles->reserve(size);
			
			getline(inp, line, '\n');
			
			for (int i = 0; i < size; i++) {
				
				getline(inp, line, '\n');
				int pos = line.find("\t", 0);
				if (pos == -1) break;
				a_str = line.substr (0, pos);
				
				int pos2 = line.find("\t", pos + 1);
				b_str = line.substr (pos + 1, pos2 - pos - 1);
				
				int pos3 = line.find("\n", pos2 + 1);
				c_str = line.substr (pos2 + 1, pos3 - pos2 - 1);
				
				Triangle* tr = new Triangle;
			
				tr->a = atoi (a_str.c_str());
				tr->b = atoi (b_str.c_str());
				tr->c = atoi (c_str.c_str());
			
				m_triangles->push_back (tr);
			}
			
			getline(inp, line, '\n');
			if (atol(line.c_str()) != 0) throw TriangleFormatExc();
			
			getline(inp, line, '\n');
			long size_a = atol (line.c_str());
			
			getline(inp, line, '\n'); 
			long size_b = atol (line.c_str());
			
			getline(inp, line, '\n');

			size = size_a * size_b;
			
			for (int i = 0; i < size; i++)
				getline(inp, line, '\n'); 
				
			getline(inp, line, '\n');
			if (atol(line.c_str()) != 0) throw ControlPointFormatExc();
				
			while (getline(inp, line, '\n')){
				if (atol(line.c_str()) != 0) break;
			};	
			
			TriangleType::const_iterator it2 
		    	= m_triangles->begin();
	
		    while (it2 != m_triangles->end()) {
	    		
	    		Triangle* el = *it2;
	    	
		    	if (((el->a) > m_points->size())
		    	 || ((el->b) > m_points->size())
		    	 || ((el->c) > m_points->size())) {
		    		throw CoordIndexExc();
		    	}
		    	
		    	Coord* point1 = (*m_points)[(el->a) - 1];
		    	Coord* point2 = (*m_points)[(el->b) - 1];
		    	Coord* point3 = (*m_points)[(el->c) - 1];
	    			
	    		TriangleCoord* trcoord = new TriangleCoord;
	    		
	    		trcoord->a = point1;
	    		trcoord->b = point2;
	    		trcoord->c = point3;
	    		
	    		m_triangleCoords->push_back(trcoord);
	    			
	    		it2++;
	    	}	
		}		
	}
	
	catch (Exc &e) {
	}
	
	catch (...) {
		cerr << "Wrong file format";
		exit(1);
	}	
}

// EOF: voxel/reader/rawreader.cpp
