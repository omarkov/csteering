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



#ifndef _FAN_ERROR
#define _FAN_ERROR

#ifdef __IRIX__
#include <stdarg.h>
#else
#include <cstdarg>
#endif

#define FAN_BACK "6"
#define FAN_COL "300"
#define FAN_COL_NORM  "\033[0G\033[0m\017"
#define FAN_COL_RED   "\033["FAN_COL"G\033["FAN_BACK"D\033[1m\033[31m"
#define FAN_COL_GREEN "\033["FAN_COL"G\033["FAN_BACK"D\033[1m\033[32m"
#define FAN_COL_TRACE "\033[1m\033[34m"
#define FAN_COL_TRACE_OUT "\033[1m\033[35m"
#define FAN_TRACE_INDENT 2
#define FAN_TRACE_INDENT_CHAR '|'

/**
 * Error Logging
 *
 * Logs different levels (FAN_ERROR,FAN_WARN,FAN_DEBUG,FAN_INFO)
 * Logs different sources (FAN_SQL,FAN_SOCKET,FAN_MISC,FAN_INIT,FAN_CONFIG)
 * Logs to different logging facilities (FAN_CONSOLE,FAN_SYSLOG,FAN_DB)
 */
class FAN_Error
{
public:
	/**
	 * Traces the caller to the current logging facility.
	 * Is automatically expanded by the FAN__BACKTRACE macro.
	 *
	 * @param function Calling function
	 * @param line Line in source file
	 * @param file Path to source file
	 * @param in true=enter function, false=leave function
	 */
	static void trace(const char * function, int line ,char * file, bool in);
	/**
	 * Logs to the current logging facility.
	 * Is automatically expanded by the FAN_xlog macro.
	 *
	 * @param level The logging level + debugging level + [color]
	 * @param function Calling function
	 * @param line Line in source file
	 * @param file Path to source file
	 * @param fmt Printf style output format
	 */
 	static void zlog(int level, const char * function, int line ,char *file, char *fmt,...);
	/**
	 * @deprecated
	 */
	static void logStdout(int, char *fmt,va_list argument);
};

#endif
