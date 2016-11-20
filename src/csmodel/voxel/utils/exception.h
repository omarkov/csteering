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
 * @file voxel/utils/exception.h
 * general exception
 *
 * base class for all defined exception classes
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <string>
#include <iostream>
#include <global.h>

/**
 * base class for all thrown exceptions
 */
class Exception {
    public:
  
    /** default constructor */
    Exception() {}

    /** 
     * exception constructor. 
     * @param msg error text
     */
    Exception(std::string msg) : m_message(msg) { 
    	std::cerr << msg << std::endl; 
    }
    
    virtual ~Exception(){}

    /**
     * returns the error message to the exception
     * @return error description
     */
    virtual const char* getMsg() {
        return m_message.c_str();
    }
    
    virtual ErrorType getErrorType() {
    	return GENERAL_ERROR;
    }

    private:
    
    /** 
     * error text 
     */
    std::string m_message;
};

/**
 * is thrown if not enough memory is available for allocation
 */
class NotEnoughMemoryException : public Exception {
    public:
  
    /**
     * constructor
     */
    NotEnoughMemoryException() : Exception("Not enough memory") {}
};

#endif // ! __EXCEPTION_H__
