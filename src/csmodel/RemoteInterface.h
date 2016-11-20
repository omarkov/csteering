// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org>
//   All rights reserved.
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

#ifndef REMOTEINTERFACE_H
#define REMOTEINTERFACE_H

typedef struct {
    double x, y, z;
} vertex_t;

typedef struct {
    unsigned int a, b, c;
} triangle_t;

typedef struct {
    unsigned int id;

    unsigned int num_vertices;
    vertex_t* vertices;

    unsigned int num_triangles;
    triangle_t* triangles;

    unsigned int u_size, v_size;
    vertex_t* control_points;
} face_t;

/*! \struct CSVERTEX
    \brief Represents a point in 3D
*/
struct CSVERTEX
{
  double x; /*!< x-coordinate */
  double y; /*!< y-coordinate */
  double z; /*!< z-coordinate */
};

/*! \struct CSTRIANGLE
    \brief Represents a triangle in 3D
    \remarks The entries are "pointers" to an array of CSVERTEX
*/
struct CSTRIANGLE
{
  int a;  /*!<  first edge  */
  int b;  /*!<  second edge */
  int c;  /*!<  third edge  */
};

/*! \typedef CSPOLE
  \brief Pole of a nurbs surface
  \remarks Since a pole is a simple point  3D, a pole has the same
    structure as a vertex, this typedef was introduced to improve readability
*/
typedef CSVERTEX CSPOLE;

/*! \struct FACE_TRGL
    \brief Represents the complete triangulation of a face, including poles
*/
struct FACE_TRGL
{
  int iSizeVertices;        /*!< number of vertices   */
  int iSizeTriangles;       /*!< number of triangles  */
  int iSizePolesU;          /*!< number of poles in u-direction */
  int iSizePolesV;          /*!< number of poles in v-direction */
  CSVERTEX *vertices;       /*!< vertices of the triangles*/
  CSTRIANGLE *triangles;    /*!< triangles */
  CSPOLE *poles;            /*!< control points (poles) of the face (NURB..) */
};

enum CADFILETYPE
{
  CADFILETYPE_UNDEFINED = 0,
  CADFILETYPE_STEP = 1,
  CADFILETYPE_IGES = 2,
  CADFILETYPE_STL = 3,
  CADFILETYPE_VRML = 4,
  CADFILETYPE_BREP = 5
};


/*
// Declaration of michail
//Type definitions
typedef struct CSVERTEX {
  double x, y, z;
};

typedef struct CSTRIANGLE {
  int a, b, c;
};

typedef struct CSPOLE {
  double x, y, z;
};

typedef struct FACE_TRGL {
  int sizeVertices;
  int sizeTriangles;
  int sizePolesU;
  int sizePolesV;
  CSVERTEX *vertices;
  CSTRIANGLE *triangles;
  CSPOLE *poles;
};
*/

#endif // REMOTEINTERFACE_H

