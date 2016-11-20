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



#ifndef _FAN_BASE64
#define _FAN_BASE64

#define CHAR64(c) (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])

/**
 * Base-64 Lib
 */
class FAN_Base64{
	static const int OK = 0;
	static const int FAIL = -1;
	static const int BUFOVER = -2;


public:
	/**
	 * Encodes a string to base-64. The destination buffer is allocated by the 
	 * function. The size increases by 1/3.
	 *
	 * Note: The string must be freed by the caller.
	 *
	 * @param in Input string
	 * @param out Reference to output buffer 
	 */
	static bool aencode64(const char *in, char **out);
	/**
	 * Encodes a string to base-64. The destination buffer is NOT allocated by
	 * this function.
	 *
	 * @param _in Input string
	 * @param inlen Length of [_in]
	 * @param _out Pre-allocated output buffer
	 * @param outmax Max length of [_out]
	 * @param outlen Reference to real length of the output buffer
	 */
	static bool encode64(const char *_in, unsigned inlen,char *_out, unsigned outmax, unsigned *outlen);
	/**
	 * Decodes a string from base-64. The destination buffer is allocated by the 
	 * function.
	 *
	 * Note: The string must be freed by the caller.
	 *
	 * @param in Base-64 string
	 * @param out Reference to output buffer
	 */
	static bool adecode64(const char *in, char **out);
	/**
	 * Decodes a string to base-64. The destination buffer is NOT allocated by
	 * this function.
	 *
	 * @param in Input string
	 * @param inlen Length of [in]
	 * @param out Pre-allocated output buffer
	 * @param outlen Reference to real length of the output buffer
	 */
	static bool decode64(const char *in, unsigned inlen, char *out, unsigned *outlen);
};

#endif
