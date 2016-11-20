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
 * @file voxel/writer/writer.h
 * factory method and writer interface for writing of an octal tree file
 * 
 * for the writer creation the static method getWriter() can be used,
 * the rest of the methods define the writer interface, all of them must
 * be implemented in all of the used writers
 * 
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __WRITER_H__
#define __WRITER_H__

#include <exception.h>
#include <octree/index_oct.h>

#include <stdio.h>

/**
 * abstract writer class
 *
 * defines the abstract methods of the writer interface and the 
 * static method getWriter() that returns a writer to a file
 */
class Writer {
    public:
    
    // ##### Exceptions ################################################

    /**
     * base class for all exceptions thrown by the writer class
     */
    class WriterException : public Exception {
        public:
        
        /** 
         * default exception constructor
         */
        WriterException() : Exception() {}
    
        /** 
         * exception constructor 
         * @param msg error message
         */
        WriterException(std::string msg) : Exception(msg) {}
    };

    /**
     * is thrown in case of a write error
     */
    class WriteFileException : public WriterException {
  
        public:
    
        /** 
         * exception constructor
         * @param fileName file name
         */
        WriteFileException(const char* fileName)
            : WriterException("Error writing file " + (std::string)fileName) {}
    };

    /**
     * is thrown if the file is in the wrong format
     */
    class FileFormatException : public WriterException {
        public:
            
        /** 
         * exception constructor
         * @param msg error message
         */
        FileFormatException(std::string msg) : WriterException(msg) {}
    };

    // ###### Writer ###################################################
    
    /**
     * data formats, for which there exists a writer
     */
    enum FormatType {
        POT, ///< pre-order traversion of octree structure 
        XPM  ///< x11 pixmap format
    };
  
    /**
     * factory method, creates the reader for the file fileName
     *
     * based on the file extension a writer for the file is created,
     * .pot- and .xpm-formats are supported, for all other formats 
     * FileFormatException is thrown
     * @param fileName name of the file, to which the octal tree model
     *  is to be written 
     * @return writer for the file
     * @exception FileFormatException file is in the wrong format
     */
    static Writer* getWriter(const char* fileName)
        throw (FileFormatException*);

    /**
     * returns the format name
     * @return the format name
     */
    virtual const char* getFormatName() = 0;

    /**
     * returns the format type
     * @return the format type
     */
    virtual const FormatType getFormatType() = 0;

    /**
     * writes the octal tree structure to the file
     * @param tree octal tree structure to be written
     * @exception WriteFileException file write error
     */
    virtual void writeFile(IndexOct* tree)
        throw (WriteFileException*) = 0;

}; // class Writer

#endif // ! WRITER_H
