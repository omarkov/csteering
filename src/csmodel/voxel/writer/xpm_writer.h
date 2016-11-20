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
 * @file voxel/writer/xpm_writer.h
 * .xpm file writer 
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __XPM_WRITER_H__
#define __XPM_WRITER_H__

#include <writer/writer.h>
#include <octree/index_oct.h>

#include <fstream>
#include <iostream>
#include <assert.h>

/**
 * .xpm file writer 
 */
class XpmWriter : public Writer {
    public:
    
    /**
     * constructor
     * @param fileName name of the .xpm file
     */
    XpmWriter(const char* fileName);

    /**
     * destructor
     */
    virtual ~XpmWriter();

    /**
     * returns the name of the format
     * @return "XPM"
     */
    virtual const char* getFormatName();

    /**
     * returns the format type
     * @return Writer::XPM
     */
    virtual const FormatType getFormatType();

    /**
     * writes cuts through the octal tree strcuture to the .xpm-file
     * @param tree the octal tree to be written
     * @exception WriteFileException file write error
     * @pre DIMENSIONS == 3
     */
    void writeFile(IndexOct* tree)
        throw (WriteFileException*);

    private:
    
    /**
     * returns the colour sign
     * @param h coordinate of the cutting axis
     * @param i coordinates of both other cutting axes
     * @return colour code of the normcell (h; i)
     */
    char getColorChar(AxIndex h, AxIndex i[2]);
  
    /**
     * returns the complete name of the file to be written with the number,
     * the file name is constructed as follows:\n
     *   <name stem of the output file>_<number>.<file extension>
     * @param number file number, corresponds to the height number 
     *  of the cutting axis
     */
    const char* getFileName(int number);

    /**
     * returns the width of the .xpm-file to be written, corresponds to the
     * number of the norm cells lying below in the direction of the octal tree
     * on the lowest plane
     * @return picture width
     */
    AxIndex getWidth();

    /**
     * writes the colour code colorChar to the output file
     * @param colorChar colour code to be written
     */
    void write(char colorChar);
 
    /**
     * writes the part of the .xpm-file that is behind the voxel table
     */
    void writeFooter();

    /**
     * writes the file header including the part till the voxel table, 
     * xpm marker and colour defintions
     * @param number file number, corresponds to the height number 
     *  of the cutting axis
     */
    void writeHeader(int number);
  
    /**
     * writes the line beginning
     */
    void writeLineBegin();
  
    /**
     * writes the line ending
     * @param isLast the last voxel line or not
     */
    void writeLineEnd(bool isLast);
 
    // ##### variables ###################################################

    /**
     * used file extension
     */
    const char* m_extension;
  
    /**
     * name stem of the output file without extension
     */
    const char* m_fileName;

    /**
     * octal tree
     */
    IndexOct* m_octree;

    /**
     * output stream
     */
    std::ofstream m_out;

    /**
     * axis, along which the cuts are made
     */
    Axis m_sectAxis;

};

#endif // ! __XPM_WRITER_H__
