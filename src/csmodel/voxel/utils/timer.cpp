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
 * file: voxel/utils/timer.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <timer.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

/**
 * start time of the timer
 */
#define START_TIME SEC_PER_DAY

// ##### Timer() #####################################################
Timer::Timer() {
    reset();
}

// ##### getTime() ###################################################
float Timer::getTime() {
    getitimer(ITIMER_REAL, &m_time);
  
    return (float)SEC_PER_DAY
         - (float)m_time.it_value.tv_sec
         - (float)m_time.it_value.tv_usec /(float)(USEC_PER_SEC);
}

// ##### getTime() ###################################################
const char* Timer::getTimeStr() {
    char buf[63]= "";

    getitimer(ITIMER_REAL, &m_time);

    long sec = SEC_PER_DAY - m_time.it_value.tv_sec;

    if (sec < 60) {
        // less than 1 minute elapsed
        int oneHundreth = m_time.it_value.tv_usec / (USEC_PER_SEC / 100);
        sprintf(buf, "%0ld.%s seconds", sec, toStr(oneHundreth, 100));
    } else if (sec < 3600) {
        // less than 1 hour elapsed
        sprintf(buf, "%0ld:%s minutes", sec / 60, toStr(sec, 60));
    } else {
        sprintf(buf, "%0ld:%s:%s h", sec / 3600, toStr(sec / 60, 60), 
            toStr(sec, 60));
    }

    return strdup(buf);
}

// ##### print() #####################################################
void Timer::print() {
    printf("%s", getTimeStr());
    reset();
}

// ##### reset() #####################################################
void Timer::reset() {
    m_time.it_value.tv_sec = SEC_PER_DAY;
    m_time.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &m_time, 0);
}

// ##### toStr() #####################################################
char* Timer::toStr(long num, long maxNum) {
    num = num % maxNum;
    maxNum--;
    int dec = 0;
  
    for (; maxNum > 0; maxNum /= 10) {
        dec++;
    }
    
    assert (dec <= 10);

    char str[10];
    
    for (int i = dec; i > 0; i--) {
        str[i-1] = char((num % 10) + '0');
        num /= 10;
    }
    
    str[dec] = '\0';

    return strdup(str);
}

// EOF: voxel/utils/timer.cpp
