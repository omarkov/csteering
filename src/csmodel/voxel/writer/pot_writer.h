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
 * @file voxel/writer/pot_writer.h
 * .pot file writer 
 *
 * linearizes the octal tree structure and writes its pre-order traversion
 * to a file
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __POT_WRITER_H__
#define __POT_WRITER_H__

#include <assert.h>

#include <writer/writer.h>
#include <octree/oct_struct.h>

/**
 * type of the data to be written
 */
typedef unsigned int Data;

/**
 * leaf node
 */
const Data LEAF = 0;

/**
 * inner node
 */
const Data INNER_NODE = 1;

/**
 * .pot file writer
 *
 * linearizes the octal tree structure and writes its pre-order traversion
 * to a file
 */
class PotWriter : public Writer, private OctStruct {
    public:
    
    /**
     * constructor
     * @param fileName name of the .pot file
     */
    PotWriter(const char* fileName);

    /**
     * destructor
     */
    virtual ~PotWriter();

    /**
     * returns the name of the format
     * @return "POT"
     */
    virtual const char* getFormatName();

    /**
     * returns the format type
     * @return Writer::POT
     */
    virtual const FormatType getFormatType();

    /**
     * writes the octal tree structure to the .pot-file
     * 
     * the octal tree is linearized, the inner nodes and leaves of the octal 
     * tree are attributed according to their type, the tree is traversed 
     * using the depth search algorithm and all of its nodes are written 
     * to a binary stream along the way, the stream is subsequently written 
     * to the .pot-file 
     * @param tree the octal tree to be written
     * @exception WriteFileException file write error
     */
    virtual void writeFile(IndexOct* tree)
        throw (WriteFileException*);

    private:
  
    /**
     * bitwise encoding and saving in the binary format
     * 
     * the attributes NodeKind:LEAF or NodeKind:INNER_NODE are encoded bitwise 
     * and saved in the binary format, at the end of the output data flush must 
     * be performed, eventually additional zeroes are added
     * 
     * @param data attribute to be written
     */
    void binWrite(Data data);

    /**
     * writes the cached data and empties the cache
     */
    void flushWrite();

    /**
     * returns the size of the cache
     * @return size of the cache
     */
    const size_t getCacheSize();

    /**
     * writes the data for the inner node
     */
    void writeInnerNode();

    /**
     * writes te data for the leaf node
     * @param color leaf colour
     */
    void writeLeaf(Color color);

    /**
     * writes the subtree node in pre-order traversion to the output file
     * @param node subtree
     */
    void writeTree(Node node);

    // ##### variables ###################################################

    /**
     * name of the output file
     */
    const char* m_fileName;

    /**
     * output file handle
     */
    FILE* m_potFile;

    /**
     * number of cached bits
     */
    size_t m_wCount;

    /**
     * cached binary value
     */
    Data m_wBinary;
};

#endif // ! __POT_WRITER_H__
