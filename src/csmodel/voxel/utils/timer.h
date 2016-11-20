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
 * @file voxel/utils/timer.h
 * timer
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <sys/time.h>

/**
 * seconds per day
 */
#define SEC_PER_DAY 24*60*60

/**
 * microseconds per second
 */
#define USEC_PER_SEC 1000*1000

/**
 * timer
 */
class Timer {
    public:
  
    /**
     * constructor
     * resets the timer
     */
    Timer();

    /**
     * returns the elapsed time in seconds (since the initialization 
     * or recent reset)
     * @return elapsed time in seconds
     */
    float getTime();

    /**
     * returns the elapsed time as a C text string
     * @return elapsed time
     * @see getTime()
     */
    const char* getTimeStr();

    /**
     * prints the elapsed time to the console and resets the timer
     */
    void print();

    /**
     * resets the timer
     */
    void reset();

    /**
     * converts the number num to a sequence of characters, maxNum defines, 
     * from what number there should be an overflow, the returned string will
     * be filled with so many zeroes as to create a number with maxNum - 1 
     * digits, i.e. toStr(65, 60) returns <tt>05</tt> , toStr(111, 100) returns 
     * <tt>11</tt>
     * @param num number to be converted
     * @param maxNum unit number, a number modulo the unit number will 
     *  be returned
     * @return number as a formatted C string of characters
     */
    char* toStr(long num, long maxNum);

    private:
  
    /**
     * remaining time
     */
    struct itimerval m_time;

};

#endif // ! __TIMER_H__
