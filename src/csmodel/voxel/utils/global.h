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
 * @file voxel/utils/global.h
 * contains general definitions and constants
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <vector>
#include <limits.h>

/**
 * switch off the assertion checking
 */
#define NDEBUG

/**
 * use the classic algorithm (consecutive scan line transformation)
 * WARNING: very slow!
 */
//#define CLASSIC_MODE

/**
 * fill the generated objects, if the option is switched off,
 * only the surfaces of the objects are generated
 */
#define FILL_SOLIDS

/**
 * use the IsIn-Polygon algorithm, generates a qualitatively 
 * different result
 * WARNING: the algorithm is slow
 */
//#define ALGORITHM_ISIN

/**
 * use the determinant check algorithm for Polygon::isInPlane()
 */
//#define ALGORITHM_CHECK_DET

/**
 * the ccw-check of the two-dimensional projection will be used 
 * for the triangle surface generation, slower, generates the same 
 * result
 * @pre Polygon::getCount() == 3
 */
//#define CCW_CHECK

/**
 * scanline transformation will simultaneously move along several axes (thus
 * allowing direct passage to diagonal neighbours)
 */
#define COMB_SCAN_LINE

/**
 * safe definition of the filling colour, the filling algorithm will use
 * the data of the neighbouring points to determine the filling colour.
 */
//#define SAFE_FILL

/**
 * a queue will be used instead of a stack, MARK_BORDER, LIMITED_STACK 
 * and MAX_RECURSION_DEPTH have the same effect 
 */
#define USE_QUEUE

/**
 * at first sets the node, that would have been filled in case of 
 * MAX_RECURSION_DEPTH, to the border colour, from which the real colour
 * can be derived, the filling can be continued with an empty stack beginning 
 * from this node, the switch is an alternative to the LIMITED_STACK, the switch
 * is relevant only if the FILL_SOLIDS switch is on
 */
//#define MARK_BORDER

/**
 * limitation of the call stacks of FillOct::fillNext(), this switch is relevant
 * only if the FILL_SOLIDS switch is on, the maximal number of calls will be 
 * limited to MAX_RECURSION_DEPTH by FillOct::fillNext()
 */
//#define LIMITED_STACK

/**
 * the ray algorithm will be used to determine the filling colour
 */
#define RAY_METHOD

/**
 * defines the number of neighbours that will be filled together (if they lie
 * in the same area), this value is relevant for particularly large areas, the
 * larger this value is, the faster can large areas be filled, yet respectively 
 * more stack space will be needed, the switch is relevant only if the switches
 * FILL_SOLIDS and {MARK_BORDER or LIMITED_STACK} are on
 */
#define MAX_RECURSION_DEPTH 65000

/**
 * the maximal square of the distance between two vectors, in case of which they 
 * are considered identical if ALGORITHM_ISIN is on
 */
#define VEC_ABS_MAX 0.75

/**
 * the maximum, determining if the point is regarded as belonging to the plane 
 * if ALGORITHM_CHECK_DET is on
 */
#define DET_MAX 0.5

/**
 * number of dimensions of the CadModel and Octree structures
 */
#define DIMENSIONS 3

/**
 * number of border points of a plane
 * @see Plane
 */
#define COUNT_VERTEX_IN_FACE 3

/**
 * the highest possible depth of the octal tree
 */
#define ABSOLUTE_MAX_DEPTH ( (8*sizeof(int) - 1) / 2 )

/**
 * the maximal distance between two points beginning from which they are not 
 * considered to be the same point
 */
#define EPSILON 1e-8

/**
 * maximum of two numbers
 */
#define MAX_VAL(x1, x2) ((x1) > (x2) ? (x1) : (x2))

/**
 * minimum of two numbers
 */
#define MIN_VAL(x1, x2) ((x1) < (x2) ? (x1) : (x2))

/**
 * the standard maximal length of the string buffer
 */
#define STR_LEN 1024

/**
 * defines if the test ray is parallel to the x-axis
 * 
 * @see OctGen::testRay()
 * @see OctGen::testLine()
 */
#define RAY_AXIS X_AXIS 

/**
 * the test ray is in +infinity
 * 
 * @see OctGen::testRay()
 */
#define RAY_DIR FORWARD

/**
 * type for point or vector coordinates
 */
typedef double Coordinate;

/**
 * coordinate axis
 */
typedef int Axis;

/**
 * 3-dimensional voxelization vector
 */
typedef std::vector<std::vector<std::vector<bool> > > Voxels;

#ifdef COMB_SCAN_LINE
/**
 * combination of several axes
 */
typedef int AxComb;
#endif

/**
 * x-axis
 */
const Axis X_AXIS= 0;

/**
 * y-axis
 */
const Axis Y_AXIS= 1;

/**
 * z-axis
 */
const Axis Z_AXIS= 2;

/**
 * type for the tree height, based on which the maximal resolution is determined
 * 
 * it is always correct to state that:
 * - height of the root node = maxTreeHeight of the tree
 * - height of a leaf node of the lowest plane = 0
 */
typedef int Height;

/**
 * the maximal possible depth
 */
const Height MAX_HEIGHT = ABSOLUTE_MAX_DEPTH;

/**
 * type for object colour
 */
typedef int Color;

/**
 * the largest colour
 */
const Color MAX_COLOR = INT_MAX - 1;

/**
 * returns the absolute value of x:\n
 *  x, if x >= 0\n
 * -x, otherwise
 */
Coordinate ABS_VAL(Coordinate x);

/**
 * comparison of two coordinates 
 */
bool EQUIVAL(Coordinate coord1, Coordinate coord2);

/**
 * sign of x:\n
 *  1, if x > 0\n
 * -1, if x < 0\n
 *  0, otherwise
 */
int SIGN(Coordinate x);

/**
 * returns i in blocks of three
 * @param i number
 * @return formatted 'i' 
 */
char* formatLarge(unsigned i);

/**
 * enumeration of all error types
 */
enum ErrorType {GENERAL_ERROR, MODEL_ERROR, MODEL_BOUNDARY_ERROR,
    OUTSIDE_OF_MODEL_ERROR, BAD_MODEL_ERROR, BAD_HEIGHT_ERROR,
    RAWREADER_ERROR, COORD_FORMAT_ERROR, TRIANGLE_FORMAT_ERROR,
    CONTROL_POINT_FORMAT_ERROR, COORD_INDEX_ERROR, VOXEL_ERROR,
    RAWREADER_VOXEL_ERROR, READER_VOXEL_ERROR, MODEL_VOXEL_ERROR,
    OCTGEN_VOXEL_ERROR};

#endif // ! __GLOBAL_H__
