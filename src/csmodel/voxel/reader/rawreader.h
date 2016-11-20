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
 * file: voxel/reader/rawreader.h
 * .raw-format reader
 *
 * @author Mikhail Prokharau
 * @date 2004
 */
 
#ifndef __RAWREADER_H__
#define __RAWREADER_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> 
#include <exception.h>
#include <../RemoteInterface.h>

/**
 *  class for reading .raw files and triangulation structures
 */
class RawReader {
    public:

    /**
     * memory buffer
     */
    static unsigned char *m_buffer;

    /**
     * old buffer size
     */
    static int m_oldSize;

    /**
     * constructor, initializes the class objects by reading
     * data from a .raw-file
     * @param input name of the .raw-file
     */ 
    RawReader(const char* input);
    
    /**
     * constructor, initializes the class objects by reading
     * data from a triangulation structure
     * @param faces the triangulation structure array  
     * @param size the array's size
     */ 
    RawReader(FACE_TRGL* faces, int size);
    
    /**
     * destructor
     */
    virtual ~RawReader();
    
    /**
     * structure defining coordinates of a point in 3d space
     */
    struct Coord {
        double x, y, z;
    };
    
    /**
     * structure defining a triangle by positions of its points in
     * the points array
     */
    struct Triangle {
        unsigned long a, b, c;
    };

    /**
     * structure defining a triangle by coordinates of its points
     * in 3d space
     */
    struct TriangleCoord {
        Coord* a; 
        Coord* b; 
        Coord* c;   
    };

    /**
     * type defining a container for points
     */
    typedef std::vector<Coord*> CoordType;
    
    /**
     * type defining a container for triangles
     */
    typedef std::vector<Triangle*> TriangleType;
    
    /**
     * type defining a container for triangles defined by their
     * 3d coordinates
     */
    typedef std::vector<TriangleCoord*> TriangleCoordType;

    /**
     * returns a container of triangles defined by their 3d coordinates
     * in space
     * @return container of triangles
     */
    TriangleCoordType* getTriangles(void);
 
    private:

    /**
     * points extracted from a file or a triangulation structure
     */
    CoordType* m_points;
    
    /**
     * triangles extracted from a file or a triangulation structure
     * defined by the previously extracted points
     */ 
    TriangleType* m_triangles; 
    
    /**
     * triangles defined by their 3d coordinates in space
     */
    TriangleCoordType* m_triangleCoords;
        
    /**
     * extracts triangles from the given .raw-file
     * @param input name of the .raw-file
     */
    void read(const char* input);
}; // class RawReader

// ##### Exceptions ################################################

/**
 * is thrown in case of a general format error
 */
class Exc : public Exception {
    public:
    
    Exc(std::string msg) : Exception(msg) {
    }
    
    virtual ~Exc() {}
    
    virtual ErrorType getErrorType() {
    	return RAWREADER_ERROR;
    }
};

/**
 * is thrown in case of a coordinate format error
 */
class CoordFormatExc: public Exc {
    public:
    
    CoordFormatExc() : Exc("Coordinate format error") {
    }
    
    virtual ~CoordFormatExc() {}
    
    virtual ErrorType getErrorType() {
    	return COORD_FORMAT_ERROR;
    }
};

/**
 * is thrown in case of a triangle format error
 */
class TriangleFormatExc: public Exc {
    public:
    
    TriangleFormatExc() : Exc("Triangle format error") {
    }
    
    virtual ~TriangleFormatExc() {}
    
    virtual ErrorType getErrorType() {
    	return TRIANGLE_FORMAT_ERROR;
    }
};

/**
 * is thrown in case of a descrepancy in the number
 * of control points
 */
class ControlPointFormatExc: public Exc {
    public:
    
    ControlPointFormatExc() : Exc("Control point format error") {
    }
    
    virtual ~ControlPointFormatExc() {}
    
    virtual ErrorType getErrorType() {
    	return CONTROL_POINT_FORMAT_ERROR;
    }
};

/**
 * is thrown in case of an attempt to access triangle coordinates 
 * via an incorrect coordinate index
 */
class CoordIndexExc: public Exc {
    public:
    
    CoordIndexExc() : Exc("Coordinate index error") {
    }
    
    virtual ~CoordIndexExc() {}
    
    virtual ErrorType getErrorType() {
    	return COORD_INDEX_ERROR;
    }
};

#endif // ! __RAW_READER_H__
