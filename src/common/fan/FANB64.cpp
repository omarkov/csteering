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

#include "FANClasses.h"

/*
** FAN_encodeBlock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void FAN_encodeBlock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = FAN_cb64[ in[0] >> 2 ];
    out[1] = FAN_cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? FAN_cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? FAN_cb64[ in[2] & 0x3f ] : '=');
}

/*
** FAN_encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
char *FAN_encode( unsigned char *vin, int size)
{
    FAN_ENTER;
    unsigned char in[3], out[4];
    int i, len;
    int pos = 0;
    int ipos = 0;
  //  int osize = size*4/3;
    int osize = (size/6+1)*8 + 2;
  /*  int test = osize / 4;
    if(test*4 != osize)
        osize = (test+1)*4;*/

    char *cout = (char*)malloc(sizeof(char)*osize+1);

    while(ipos < size)
    {
        len = 0;
        for( i = 0; i < 3; i++ ) {
            if(ipos < size)
            { 
                in[i] = vin[ipos++];
		len++;

	    } else {
                in[i] = '\0';
            }
        }

        if( len ) {
            FAN_encodeBlock( in, out, len );
            for( i = 0; i < 4; i++ ) {
                cout[pos++] = out[i];
            }
        }
    }

    cout[pos] = '\0';
    char *ret = (char*)malloc(strlen(cout) + 1);
    strcpy(ret, cout);
    free(cout);
    
    FAN_RETURN ret;
}

/*
** FAN_decodeBlock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void FAN_decodeBlock( unsigned char in[4], unsigned char out[3] )
{   
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** FAN_decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
unsigned char *FAN_decode( char *cin, int osize )
{
    FAN_ENTER;
    unsigned char in[4], out[3], v;
    int i, len;

    int pos = 0;
    int ipos = 0;
    int isize = 0;

//    while(cin[isize] != '\0') isize++;
    isize = strlen(cin);
    
    unsigned char *bout = (unsigned char*)malloc(sizeof(unsigned char)*osize);

    while(ipos < isize)
    {
        for( len = 0, i = 0; i < 4 && ipos < isize; i++ ) {
            v = 0;
            while( ipos < isize && v == 0 ) {
                v = (unsigned char) cin[ipos++];
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : FAN_cd64[ v - 43 ]);
                if( v ) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if( ipos <= isize ) {
                len++;
                if( v ) {
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = '\0';
            }
        }
        if( len ) {
            FAN_decodeBlock( in, out );
            for( i = 0; i < len - 1; i++ ) {
                bout[pos++] = out[i];
            }
        }
    }
    FAN_RETURN bout;
}

