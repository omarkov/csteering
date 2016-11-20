/*
 * FAN - Framework for Applications in Networks
 * Copyright (C) 2004 FreshX [dominik@freshx.de]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*

	error loggin facility

*/

#define FAN_OUTSTRING_SIZE 180

#ifdef __IRIX__
#include <stdarg.h>
#else
#include <cstdarg>
#endif

#include "FANClasses.h"


pthread_mutex_t FAN_traceBackMut = PTHREAD_MUTEX_INITIALIZER;
FAN_Hash *FAN_traceBackHash;

void FAN_Error::trace(const char * funcName, int funcLine, char * funcFile, bool in)
{
	pthread_mutex_lock(&FAN_traceBackMut);
	int id = FAN__getThreadId();

	char *cid = NULL;
	asprintf(&cid, "%d", id);
	char *cDepth = NULL;
	if(FAN_traceBackHash == NULL)
		FAN_traceBackHash = new FAN_Hash(false);
	else
		cDepth = FAN_traceBackHash->getValueNoTB(cid);

	int depth = 0;
	if(cDepth != NULL)
	    depth = atoi(cDepth);

	if(!in)
		depth-=FAN_TRACE_INDENT;

	char *ind = new char[depth+1];
	for(int i=0; i<depth; i++)
	{
	    if(i % FAN_TRACE_INDENT == 0)
	      ind[i] = FAN_TRACE_INDENT_CHAR;
	    else
	      ind[i] = ' ';
	}
	ind[depth] = 0;

	if(in)
	{
		printf(FAN_COL_TRACE);
	}else
	{
		printf(FAN_COL_TRACE_OUT);
	}
	printf("[%d]%s%s:%s(%i)\n",id, ind, funcFile, funcName, funcLine); // source print
        printf(FAN_COL_NORM);
	delete []ind;

	if(in)
	    depth+=FAN_TRACE_INDENT;

	char *newDepth = NULL;
	asprintf(&newDepth, "%d", depth);
	FAN_traceBackHash->insertNoTB(cid, newDepth);

	free(newDepth);
	free(cid);

	pthread_mutex_unlock(&FAN_traceBackMut);
}

void FAN_Error::zlog(int level, const char * funcName, int funcLine, char * funcFile, char *fmt,...)
{
        char date[60],bs[FAN_OUTSTRING_SIZE];
        va_list argument;
        time_t t;
	FAN *app = FAN::app;

	if(app == NULL)
	{
             time(&t);
             strftime(date,59,"%Y/%m/%d %T",localtime(&t));
             printf("[ STARTUP ]  %s(%i) in %s : ",funcName, funcLine, funcFile); // source print
             va_start(argument, fmt);
             vprintf(fmt, argument);
             va_end(argument); 

	     printf("\n");

	     return;
	}else if(app->theDaemon != NULL && !app->theDaemon->binded())
	{
             time(&t);
             strftime(date,59,"%Y/%m/%d %T",localtime(&t));
             printf("[ SHUTTING DOWN ]  %s(%i) in %s : ",funcName, funcLine, funcFile); // source print
             va_start(argument, fmt);
             vprintf(fmt, argument);
             va_end(argument); 

	     printf("\n");

	     return;
	}


	int dblevel = app->DEBUG_LVL & (~app->DEBUG_IGNORE);

	memset(bs,0,FAN_OUTSTRING_SIZE);

        if((level&app->LOG_LVL) >0)
        {
                if((level&FAN_DEBUG) > 0 && (level&dblevel) == 0) 
                {
                        return;
                }
                
		if((level&FAN_QUIET) == 0) {        // time print
                        time(&t);
                        strftime(date,59,"%Y/%m/%d %T",localtime(&t));
                        sprintf(bs,"[%s|0x%.5x] %s(%i) in %s : ",date,level,funcName, funcLine, funcFile); // source print
                }

                if((level&FAN_GREEN) > 0)
                {
                        printf(FAN_COL_GREEN);
                }else if((level&FAN_RED) > 0)
                {
                        printf(FAN_COL_RED);
                }
    
        printf(bs);

        va_start(argument, fmt);
        vprintf(fmt, argument);
        va_end(argument); 

        if((level&FAN_NOCR) == 0) 
		printf("\n");
        if((level&(FAN_GREEN | FAN_RED)) > 0)
                printf(FAN_COL_NORM);
        }

	if((level&FAN_EXIT) > 0) exit(1);
}

// --OM: non-gcc compilers don't support variadic macros
#ifndef __GNUC__
void FAN_xlog(int level, char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    FAN_Error::zlog(level, "unknown", -1, "unknown", fmt, args);
    va_end(args);
}

void FAN_err(char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    FAN_Error::zlog(FAN_ERROR, "unknown", -1, "unknown", fmt, args);
    va_end(args);
}

void FAN_plog(int level, char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    FAN_Error::zlog(FAN_ERROR | level, "unknown", -1, "unknown", fmt, args);
    va_end(args);
}
#endif // __GNU__

void FAN_Error::logStdout(int, char *fmt,va_list argument)
{
}
