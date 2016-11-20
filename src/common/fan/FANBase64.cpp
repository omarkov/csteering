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
 
char basis_64[] =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????";


int index_64[128] = {
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
          52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
          -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
          15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
          -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
          41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
        };


bool FAN_Base64::aencode64(const char *in, char **out)
{	
	FAN_ENTER;

	if(in != NULL)
	{
		if(strlen(in) == 0)
		{
			*out = NULL;
			FAN_RETURN false;
		}

		unsigned *len = (unsigned*)malloc(sizeof(unsigned));
		unsigned inlen  = strlen(in);
		unsigned outlen = (inlen<<3)/6+10;
		int ret;
		*out = (char*)malloc(outlen);

		ret = encode64(in, strlen(in), (char*)*out, outlen, len);
		free(len);
		FAN_RETURN ret;
	}else
	{
		*out = NULL;
		FAN_RETURN false;
	}
}


bool FAN_Base64::encode64(const char *_in, unsigned inlen,
	                    char *_out, unsigned outmax, unsigned *outlen)
{
  FAN_ENTER;

  const unsigned char *in = (const unsigned char *) _in;
  unsigned char  *out = (unsigned char *) _out;
  unsigned char   oval;
  //char           *blah;
  unsigned        olen;

  olen = (inlen + 2) / 3 * 4;
  if (outlen)
      *outlen = olen;
  if (outmax < olen)
  {
      FAN::app->errorNo = BUFOVER;
      FAN_RETURN false;
  }

  //blah = (char *) out;
  while (inlen >= 3)
  {
      /* user provided max buffer size; make sure we don't go over it */
      *out++ = basis_64[in[0] >> 2];
      *out++ = basis_64[((in[0] << 4) & 0x30) | (in[1] >> 4)];
      *out++ = basis_64[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
      *out++ = basis_64[in[2] & 0x3f];
      in += 3;
      inlen -= 3;
  }
  if (inlen > 0)
  {
/* user provided max buffer size; make sure we don't go over it */
      *out++ = basis_64[in[0] >> 2];
      oval = (in[0] << 4) & 0x30;
      if (inlen > 1)
          oval |= in[1] >> 4;
      *out++ = basis_64[oval];
      *out++ = (inlen < 2) ? '=' : basis_64[(in[1] << 2) & 0x3c];
      *out++ = '=';
  }

  if (olen < outmax)
      *out = '\0';

  FAN_RETURN true;
}

bool FAN_Base64::adecode64(const char *in, char **out)
{	
	FAN_ENTER;
	if(in != NULL)
	{
		if(strlen(in) == 0)
		{
			*out = NULL;
			FAN_RETURN false;
		}

		unsigned *len = (unsigned*)malloc(sizeof(unsigned));
		unsigned inlen  = strlen(in);
		unsigned outlen = (inlen>>3)*6+10;
		int ret;
		*out = (char*)malloc(outlen);

        	ret = decode64(in, inlen, (char*)*out, len);

		free(len);
		FAN_RETURN ret;
	}else
	{
		*out = NULL;
		FAN_RETURN false;
	}
}

bool FAN_Base64::decode64(const char *in, unsigned inlen, char *out, unsigned *outlen)
{
  FAN_ENTER;

  unsigned        len = 0,
                  lup;
  int             c1,
                  c2,
                  c3,
                  c4;

  if (in[0] == '+' && in[1] == ' ')
      in += 2;

  if (*in == '\0')
  {
      FAN_RETURN false;
  }

  for (lup = 0; lup < inlen / 4; lup++)
  {
      c1 = in[0];
      if (CHAR64(c1) == -1)
      {
          FAN_RETURN false;
      }
      c2 = in[1];
      if (CHAR64(c2) == -1)
      {
          FAN_RETURN false;
      }
      c3 = in[2];
      if (c3 != '=' && CHAR64(c3) == -1)
      {
          FAN_RETURN false;
      }
      c4 = in[3];
      if (c4 != '=' && CHAR64(c4) == -1)
      {
          FAN_RETURN false;
      }
      in += 4;
      *out++ = (CHAR64(c1) << 2) | (CHAR64(c2) >> 4);
      ++len;
      if (c3 != '=')
      {
          *out++ = ((CHAR64(c2) << 4) & 0xf0) | (CHAR64(c3) >> 2);
          ++len;
          if (c4 != '=')
          {
              *out++ = ((CHAR64(c3) << 6) & 0xc0) | CHAR64(c4);
              ++len;
          }
      }
  }

  *out = 0;
  *outlen = len;

  FAN_RETURN true;
}
