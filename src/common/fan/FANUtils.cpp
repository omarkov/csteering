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



#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include "FANClasses.h"

void *FAN_getApp()
{
   FAN_ENTER;
   FAN_RETURN pthread_getspecific(FAN::app->threadFAN);
}

void FAN_recycleMsg(FAN_Com *com, FAN_Msg *msg)
{
	com->msgRecycler->push(msg);
}
FAN_Msg *FAN_newMsg(FAN_Com *com)
{
   FAN_ENTER;
   FAN_Msg *msg = NULL;
   if(com != NULL)
   {
      msg = (FAN_Msg*)com->msgRecycler->pop();
   }
   if(msg ==  NULL)
   {
      msg = new FAN_Msg;
   }
   FAN_RETURN msg;
}
void *FAN_sendMessage (FAN_Com *com, char *type, void *value)
{
   FAN_ENTER;
   FAN_RETURN FAN_sendMessage(com, type, type, value);
}

void *FAN_sendMessage(FAN_Com *com, char *type, char *replyType, void *value)
{
   FAN_ENTER;
   FAN_Com *reply = FAN_initMessages();

   FAN_postMessage(com, type, reply, replyType, value);
   FAN_Msg *msg = FAN_peekMessage(reply);

   void *ret = msg->value;

   delete msg;
   FAN_freeMessages(reply); 

   FAN_RETURN ret;
}

void FAN_postMessage(FAN_Com *com, char *type, FAN_Com* reply, void *value)
{
   FAN_ENTER;
   FAN_postMessage(com, type, reply, type, value);
   FAN_RETURN;
}

void FAN_postMessage(FAN_Com *com, char *type, FAN_Com *reply, char *replyType, void *value)
{ 
    FAN_ENTER;
    FAN_Msg *msg = FAN_newMsg(com);
    pthread_t thread = pthread_self();
    msg->type = type;
    msg->value = value;
    msg->threadid = (int)thread;
    msg->replyType = replyType;
    msg->reply = reply;
    pthread_mutex_lock(&(com->com_mutex));
    com->MsgQueue->push(msg);
    pthread_mutex_unlock(&(com->com_mutex));
    pthread_cond_signal(&(com->com_cv));
    FAN_RETURN;
}

FAN_Com *FAN_initMessages()
{
    	FAN_ENTER;
        FAN_Com *com = new FAN_Com;

        pthread_mutex_init(&(com->com_mutex), NULL);
        pthread_mutex_init(&(com->lock_mutex), NULL);
        pthread_cond_init (&(com->com_cv), NULL);
        com->MsgQueue = new FAN_Queue();
	com->msgRecycler = new FAN_Queue();

        FAN_RETURN com;
}

void FAN_freeMessages(FAN_Com *com)
{
	FAN_ENTER;	
        pthread_mutex_destroy(&(com->com_mutex));
        pthread_cond_destroy(&(com->com_cv));
        delete(com->MsgQueue);
	FAN_Msg *msg = NULL;
	while((msg = (FAN_Msg*)com->msgRecycler->pop()) != NULL) delete msg;
	delete(com->msgRecycler);

        delete com;
	FAN_RETURN;
}

FAN_Msg *FAN_peekMessage(FAN_Com *com)
{
	FAN_ENTER;	

        pthread_mutex_lock(&(com->com_mutex));
        FAN_Msg *msg = (FAN_Msg*)com->MsgQueue->pop();

	while(msg == NULL)
	{
        	pthread_cond_wait(&(com->com_cv), &(com->com_mutex));
        	msg = (FAN_Msg*)com->MsgQueue->pop();
	}
        pthread_mutex_unlock(&(com->com_mutex));

        FAN_RETURN msg;
}


/*  FMT:
 *   
 *  basic types:
 *    int, float, double, char, byte, long
 *
 *  struct:
 *    {int, float} 
 *
 *  array:
 *    [12]
 *   
 *  example
 *
 *  int;{int,int,int}[10]
 * 
 */
char *FAN_aBin2Base64(char *fmt, unsigned char *arg)
{
        FAN_ENTER;
        int type, size;
	char *types;
	char *txt;

	types = fmt;

	int asize = 1;

	type = FAN_getNextType(&types, &txt, &asize);

	if(type == FAN_STRUCT)
		size = FAN_getParamSize(txt, true);
	else
		size = FAN_getParamSize(txt, false);

        if(arg != NULL)
        {
                FAN_RETURN FAN_encode(arg, size);
        }else
		FAN_RETURN NULL;
}

unsigned char *FAN_aBase642Bin(char *fmt, char *arg)
{
	FAN_ENTER;	
	FAN_RETURN FAN_aBase642Bin(fmt, arg, NULL);
}
unsigned char *FAN_aBase642Bin(char *fmt, char *arg, char *re)
{
        FAN_ENTER;
        int type, size;
	char *types;
	char *txt;
	unsigned char *binary = NULL;


	types = fmt;

	int asize = 0;
	type = FAN_getNextType(&types, &txt, &asize);

	if(asize < 1)
		asize = 1;

	size = 0;

	if(type == FAN_STRUCT)
		size = FAN_getParamSize(txt, true) * asize;
	else if(type == FAN_CHAR && asize > 1)
		size = FAN_getParamSize(txt, false) + 1;
	else
		size = FAN_getParamSize(txt, false);

	binary = FAN_decode(arg, size);
	
	if(asize > 1 && type == FAN_CHAR)
	{
		char *txt = (char*)binary;
		txt[asize] = '\0';
	}

	if(re != NULL)
	{
		char *le = FAN::app->config->getValue("endian");
	
		if(le != NULL && strcmp(le, re) != 0)
		{
			if(type == FAN_STRUCT)
				FAN_reverseByteOrder((unsigned char*)binary, txt-1, false);
			else
				FAN_reverseByteOrder((unsigned char*)binary, txt, false);
		}
	}


	FAN_RETURN binary;
}

int FAN_vbinaryPush(int sd, char *fmt, va_list argument)
{
        FAN_ENTER;
        unsigned char *arg;
        int i, type, size;
	char *types;
	char *txt;
	int err = 0;

	types = fmt;

	int params = FAN_getParamCount(fmt);

	FAN_swrite(sd, fmt);
	FAN_swrite(sd, "\n");

	/*
	int len = 0;
	int t_len = 0;
	char buf[3];
	while((len = read(sd, buf, 3-len)) > 0) t_len += len;
	if(len < 0 || t_len < 3)
	{
		return 0;
	}
	*/
	int asize;
	
        for(i=0; i<params; i++)
        {
		asize = 1;
		type = FAN_getNextType(&types, &txt, &asize);
		
		switch(type)
		{
                        case FAN_INT:    if(asize > 1)
                                                arg = (unsigned char*)va_arg(argument, int*);
                                         else
                                         {
                                                int t = va_arg(argument, int);
                                                arg = (unsigned char*)&t;
                                         }

                                         size = sizeof(int) * asize;
                                         break;
                        case FAN_FLOAT:  if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, double*);
                                         else
                                         {
                                                 double t = va_arg(argument, double);
                                                 arg = (unsigned char*)&t;
                                         }

                                         size = sizeof(double) * asize;
                                         break;
                        case FAN_DOUBLE: if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, double*);
                                         else
                                         {
                                                 double t = va_arg(argument, double);
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(double) * asize;
                                         break;
                        case FAN_BYTE:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, unsigned char*);
                                         else
                                         {
					         unsigned char t = va_arg(argument, int); // --OM: unsigned gets auto-promoted to int
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(unsigned char) * asize;
                                         break;
                        case FAN_CHAR:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, char*);
                                         else
                                         {
                                                 char t = va_arg(argument, int); // --OM: unsigned gets auto-promoted to int
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(char) * asize;
                                         break;
                        case FAN_STRUCT: arg = (unsigned char*)va_arg(argument, unsigned char*);
                                         size = FAN_getParamSize(txt, true) * asize;
                                         break;
                        case FAN_LONG:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, long*);
                                         else
                                         {
                                                 long t = va_arg(argument, long);
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(long) * asize;
                                         break;
			default:	 arg = NULL; 
					 size = 0;
					 break;
		}
                if(arg != NULL)
                {
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "vbinaryPush SIZE: %d", size);
                        err = err < 0 ? err : FAN_swrite(sd, arg, size);
                }
        }

        FAN_RETURN err;
}


int FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument)
{
        FAN_ENTER;
        unsigned char *arg;
        char *base64arg;
        int i, type, size;
	char *types;
	char *txt;
	int err = 0;

	types = fmt;

        err = err < 0 ? err : FAN_swrite(sd, fkt);
        err = err < 0 ? err : FAN_swrite(sd, "(");

	int params = FAN_getParamCount(fmt);

	if(params > 0)
	{
	        err = err < 0 ? err : FAN_swrite(sd, "\"");
	
		FAN_Base64::aencode64(fmt, &base64arg);
		if(base64arg != NULL)
		{
			err = err < 0 ? err : FAN_swrite(sd, base64arg);
			free(base64arg);
		}
		err = err < 0 ? err : FAN_swrite(sd, "\"");
	}

	int asize;
	
        for(i=0; i<params; i++)
        {
		asize = 1;
		type = FAN_getNextType(&types, &txt, &asize);
		
		switch(type)
		{
                        case FAN_INT:    if(asize > 1)
                                                arg = (unsigned char*)va_arg(argument, int*);
                                         else
                                         {
                                                int t = va_arg(argument, int);
                                                arg = (unsigned char*)&t;
                                         }

                                         size = sizeof(int) * asize;
                                         break;
                        case FAN_FLOAT:  if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, double*);
                                         else
                                         {
                                                 double t = va_arg(argument, double);
                                                 arg = (unsigned char*)&t;
                                         }

                                         size = sizeof(double) * asize;
                                         break;
                        case FAN_DOUBLE: if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, double*);
                                         else
                                         {
                                                 double t = va_arg(argument, double);
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(double) * asize;
                                         break;
                        case FAN_BYTE:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, unsigned char*);
                                         else
                                         {
					         unsigned char t = va_arg(argument, int); // --OM: unsigned gets auto-promoted to int
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(unsigned char) * asize;
                                         break;
                        case FAN_CHAR:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, char*);
                                         else
                                         {
                                                 char t = va_arg(argument, int); // --OM: unsigned gets auto-promoted to int
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(char) * asize;
                                         break;
                        case FAN_STRUCT: arg = (unsigned char*)va_arg(argument, unsigned char*);
                                         size = FAN_getParamSize(txt, true) * asize;
                                         break;
                        case FAN_LONG:   if(asize > 1)
                                                 arg = (unsigned char*)va_arg(argument, long*);
                                         else
                                         {
                                                 long t = va_arg(argument, long);
                                                 arg = (unsigned char*)&t;
                                         }
                                         size = sizeof(long) * asize;
                                         break;
			default:	 arg = NULL; 
					 break;
		}
                if(arg != NULL)
                {
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "vrpc SIZE: %d", size);
                        err = err < 0 ? err : FAN_swrite(sd, "\"");

                        base64arg = FAN_encode(arg, size);
                        if(base64arg != NULL)
                        {
                                err = err < 0 ? err : FAN_swrite(sd, base64arg);
                                free(base64arg);
                        }
                        err = err < 0 ? err : FAN_swrite(sd, "\"");

                        if(i+1 < params)
                        {
                                err = err < 0 ? err : FAN_swrite(sd, ",");
                        }
                }
        }
        err = err < 0 ? err : FAN_swrite(sd, ")\n");

        int ret = 0;
	if(err >= 0)
	{
		ret = hash->readFromStream(sd, 1, false);
	}else
	{
		ret = err;
	}
        FAN_RETURN ret;
}

unsigned char *FAN_reverseByteOrder(unsigned char *bytes, char *param, bool isStruct)
{
	FAN_ENTER;	
	char *types;
	char *txt;
	types = param;
	int count = FAN_getParamCount(param);
	int type = 0;
	// unused? --OM
	// int size = 0;
	int asize;
	int x,j;

	unsigned char* temp = new unsigned char[4];

	if(count > 1 && ! isStruct)
		count = 1;
	
	for(int i=0; i < count; i++)
	{
		asize = 1;

		type = FAN_getNextType(&types, &txt, &asize);	

		switch(type)
		{
			case FAN_INT:    
					 for(j=0; j < asize; j++)
					 {
						 for(x=0; x < 2; x++)
						 	temp[x] = bytes[x];
						 bytes[0]=bytes[3];
						 bytes[1]=bytes[2];
						 for(x=0; x < 2; x++)
							 bytes[3-x] = temp[x];

						 bytes += 4;
					 }
					 break;
			case FAN_LONG:
			case FAN_FLOAT: 
			case FAN_DOUBLE: 
					 for(j=0; j < asize; j++)
					 {
						 for(x=0; x < 4; x++)
						 	temp[x] = bytes[x];
						 bytes[0]=bytes[7];
						 bytes[1]=bytes[6];
						 bytes[2]=bytes[5];
						 bytes[3]=bytes[4];
						 for(x=0; x < 4; x++)
							 bytes[7-x] = temp[x];

						 bytes += 8;
					 }
					 break;
			case FAN_BYTE:
			case FAN_CHAR:
					 for(j=0; j < asize; j++)
					 {
						 bytes += 1;
					 }
					 break;
			case FAN_STRUCT: 
					 for(j=0; j < asize; j++)
					 {
						 bytes = FAN_reverseByteOrder(bytes, txt, true);
					 }
					 break;

			default : break;
		}
	}

	delete[] temp;
	FAN_RETURN bytes;
}

int FAN_getParamSize(char *param, bool isStruct)
{
	FAN_ENTER;

	char *types;
	char *txt;
	types = param;
	int count = FAN_getParamCount(param);
	int type = 0;
	int size = 0;
	int asize;

	if(count > 1 && ! isStruct)
		count = 1;
	
	for(int i=0; i < count; i++)
	{
		asize = 1;
		type = FAN_getNextType(&types, &txt, &asize);	

		switch(type)
		{
			case FAN_INT:    size += sizeof(int) * asize;
					 break;
			case FAN_FLOAT:  size += sizeof(double) * asize;
					 break;
			case FAN_DOUBLE: size += sizeof(double) * asize;
					 break;
			case FAN_BYTE:   size += sizeof(unsigned char) * asize;
					 break;
			case FAN_CHAR:   size += sizeof(char) * asize;
					 break;
			case FAN_STRUCT: size += FAN_getParamSize(txt, true) * asize;
					 break;
			case FAN_LONG:   size += sizeof(long) * asize;
					 break;
			default:	 break;
		}
	}

	FAN_RETURN size;
}

int FAN_getNextType(char **types, char **txt, int *asize)
{
	FAN_ENTER;

	char *str;
	str = *types;
	*txt = str;
	int len = strlen(str);
	int depth = 0;
	int i=0, n=0;
	bool notready = true;
	bool isArray = false;
	int count = 0;

	while(notready && i<len && depth >= 0)
	{
	  if(*(str+i) == '{')
		  depth ++;
	  else if(*(str+i) == '}')
		  depth --;
	  else if(depth == 0 && *(str+i) == '[')
	  {
		 isArray = true;
		 count = atoi(str+i+1);
		 n = i;
	  }
	  else if(depth == 0 && (*(str+i) == ';' || *(str+i) == '['))
	  {
		  notready = false;
	  }
	  i++;
	}
	if(i<len)
  	  i--;

	if(isArray)
		*asize = count;
	else
	{
		*asize = 1;
		n = i;
	}

	*types += i+1;

	if(n==3 && strncmp(str, "int", 3) == 0)
	{
		FAN_RETURN FAN_INT;
	}

	else if(n==4)
	{
		if(strncmp(str, "byte", 4) == 0)
		{
			FAN_RETURN FAN_BYTE;
		}
		else if(strncmp(str, "long", 4) == 0)
		{
			FAN_RETURN FAN_LONG;
		}
		else if(strncmp(str, "char", 4) == 0)
		{
			FAN_RETURN FAN_CHAR;
		}
	}
	else if(n==5 && strncmp(str, "float", 5) == 0)
	{
		FAN_RETURN FAN_FLOAT;
	}

	else if(n==6 && strncmp(str, "double", 6) == 0)
	{
		FAN_RETURN FAN_DOUBLE;
	}


	if(n>2 && strncmp(str, "{", 1) == 0)
	{
		*txt+=sizeof(char);
		FAN_RETURN FAN_STRUCT;
	}

	FAN_RETURN 0;
}

int FAN_getParamCount(char *fmt)
{
	FAN_ENTER;

	unsigned int i;
	int count = 1;
	int depth = 0;

	for(i=0; i<strlen(fmt); i++)
	{
		if(*(fmt+i) == '{')
		  	depth++;
		else if(*(fmt+i) == '}')
		  	depth--;
		else if(depth==0 && *(fmt+i) == ';')
		  	count ++;

		if(depth < 0)
		{
			FAN_RETURN count;
		}
	}

	FAN_RETURN count;
}

int FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, int params, va_list argument)
{
        FAN_ENTER;
        char *arg;
        char *base64arg;
        int i;
	int err = 0;

        err = FAN_swrite(sd, fkt);
        err = err < 0 ? err : FAN_swrite(sd, "(");

        for(i=0; i<params; i++)
        {
                arg = va_arg(argument, char*);

                if(arg != NULL)
                {
                        err = err < 0 ? err : FAN_swrite(sd, "\"");

                        FAN_Base64::aencode64(arg, &base64arg);
                        if(base64arg != NULL)
                        {
                                err = err < 0 ? err : FAN_swrite(sd, base64arg);
                                free(base64arg);
                        }
                        err = err < 0 ? err : FAN_swrite(sd, "\"");

                        if(i+1 < params)
                        {
                                err = err < 0 ? err : FAN_swrite(sd, ",");
                        }
                }
        }
        err = err < 0 ? err : FAN_swrite(sd, ")\n");

	int ret = 0;
	if(err >= 0)
	{
          ret = hash->readFromStream(sd, 1, false);
	}else
	{
	  ret = err;
	}
        FAN_RETURN ret;
}

int FAN_rpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, ...)
{
        FAN_ENTER;
        va_list argument;

        va_start(argument, fmt);
        int ret = FAN_vrpc(hash, sd, fkt, fmt, argument);
        va_end(argument);
	FAN_RETURN ret;
}

int FAN_rpc(FAN_Hash *hash, int sd, char *fkt, int params, ...)
{
        FAN_ENTER;
        va_list argument;

        va_start(argument, params);
        int ret = FAN_vrpc(hash, sd, fkt, params, argument);
        va_end(argument);
	FAN_RETURN ret;
}

int FAN_getClientSocket(char *hostname, int port)
{
        FAN_ENTER;
        int sd = 0;
        struct sockaddr_in pin;
        struct hostent *hp;

        /* go find out about the desired host machine */
        hp = ( struct hostent *)gethostbyname(hostname);

	if(hp == NULL)
	{
		FAN_RETURN 0;
	}

        /* fill in the socket structure with host information */
        memset(&pin, 0, sizeof(pin));

        pin.sin_family = AF_INET;
        pin.sin_port = htons(port);
//      pin.sin_addr.s_addr = htonl(hp->h_addr);
        memcpy(&pin.sin_addr, hp->h_addr, hp->h_length);


        /* grab an Internet domain socket */
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                FAN_xlog(FAN_DEBUG|FAN_SOCKET,"socket");
                FAN_RETURN 0;
        }

        /* connect to PORT on HOST */
        if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
                FAN_xlog(FAN_DEBUG|FAN_SOCKET,"connect");
                FAN_RETURN 0;
        }

        if(sd > 0)
	{
                FAN_RETURN sd;
	}
        else
	{
                FAN_RETURN 0;
	}
}

bool FAN_aGetBinaryParam(char **allParams, void **param, int pos)
{
        FAN_ENTER;

        int size = (int)*allParams;

        if(pos >= size && size > 0)
        {
                // xlog(ERROR, "aGetParam: too few Arguments");
                *param = NULL;

                FAN_RETURN false;
        }

        pthread_t thread = pthread_self();
        if(thread > 0)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

                if(conf != NULL)
                {
			char *templ = NULL;
			char *types = NULL;
			char *txt = NULL;
			int type = 0;
			FAN_Base64::adecode64(*(allParams+1), &templ);
			types = templ;

			if(templ == NULL)
			{
				FAN_RETURN false;
			}

			int count = FAN_getParamCount(templ);
			if(pos > count)
			{
				free(templ);
				FAN_RETURN false;
			}

			int i;
			int asize = 0;
		        for(i=0; i<=pos-1; i++)
        		{
			  	type = FAN_getNextType(&types, &txt, &asize);
			}

			if(asize < 1)
				asize = 1;

			int size = 0;

			if(type == FAN_STRUCT)
				size = FAN_getParamSize(txt, true) * asize;
			else if(type == FAN_CHAR && asize > 1)
				size = FAN_getParamSize(txt, false) + 1;
			else
				size = FAN_getParamSize(txt, false);

			*param = FAN_decode(*(allParams+pos+1), size);
			
			if(asize > 1 && type == FAN_CHAR)
			{
				char *txt = (char*)*param;
				txt[asize] = '\0';
			}

			char *le = conf->config->getValue("endian");
			char *re = conf->config->getValue("remoteEndian");
			if(le != NULL && re != NULL && strcmp(le, re) != 0)
			{
				if(type == FAN_STRUCT)
					FAN_reverseByteOrder((unsigned char*)*param, txt-1, false);
				else
					FAN_reverseByteOrder((unsigned char*)*param, txt, false);
			}

			FAN_xlog(FAN_DEBUG | FAN_SOCKET, "done %d", type);

			free(templ);

                        if(*param != NULL)
			{
                                FAN_RETURN true;
			}
                        else
			{
                                FAN_RETURN false;
			}
                }else
                {
                        FAN_xlog(FAN_ERROR, "aGetParam: threadFAN not found!");
                }
        }else
        {
                FAN_xlog(FAN_ERROR, "aGetParam: no valid thread id");
        }

        FAN_RETURN false;
}


bool FAN_aGetParam(char **allParams, char **param, int pos)
{
        FAN_ENTER;


        int size = (int)*allParams;

        if(pos >= size)
        {
                // xlog(ERROR, "aGetParam: too few Arguments");
                *param = NULL;

                FAN_RETURN false;
        }

        pthread_t thread = pthread_self();
        if(thread > 0)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

                if(conf != NULL)
                {
                        if(conf->clearText)
                        {
                                *param = strdup((char*)*(allParams+pos+1));
                        }else
                        {
                                FAN_Base64::adecode64(*(allParams+pos+1), param);
                        }

                        if(*param != NULL)
			{
                                FAN_RETURN true;
			}
                        else
			{
                                FAN_RETURN false;
			}
                }else
                {
                        FAN_xlog(FAN_ERROR, "aGetParam: threadFAN not found!");
                }
        }else
        {
                FAN_xlog(FAN_ERROR, "aGetParam: no valid thread id");
        }

        FAN_RETURN false;
}

void FAN_setLogs(char *from)
{
	FAN_ENTER;
	// unused? --OM
	// int init = 0;

	FAN *app = FAN::app;

        if(app->config->checkKey("debug_init", "true") && 
		(app->DEBUG_LVL & FAN_INIT) == 0)
        {
                app->DEBUG_LVL |= FAN_INIT;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +INIT", from);
        }

        if(app->config->checkKey("log_error", "false"))
	{
                app->LOG_LVL ^= FAN_ERROR;
                FAN_xlog(FAN_ERROR| FAN_INIT,"%s: ERROR off", from);
	}

        if(app->config->checkKey("log_debug", "true"))
	{
                app->LOG_LVL |= FAN_DEBUG;
                FAN_xlog(FAN_ERROR| FAN_INIT,"%s: DEBUG on", from);
	}

        if(app->config->checkKey("log_info", "true"))
	{
                app->LOG_LVL |= FAN_INFO;
                FAN_xlog(FAN_ERROR| FAN_INIT,"%s: INFO on", from);
	}

        if(app->config->checkKey("log_warn", "true"))
	{
                app->LOG_LVL |= FAN_WARN;
                FAN_xlog(FAN_ERROR| FAN_INIT,"%s: WARN on", from);
	}

        if(app->config->checkKey("debug_sql", "true") && 
		(app->DEBUG_LVL & FAN_SQL) == 0)
        {
                app->DEBUG_LVL |= FAN_SQL;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +SQL", from);
        }

        if(app->config->checkKey("debug_all", "true"))
        {
                app->DEBUG_LVL |= FAN_ALL;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +ALL", from);
        }

      	if(app->config->checkKey("debug_misc", "true") && 
		(app->DEBUG_LVL & FAN_MISC) == 0)
        {
                app->DEBUG_LVL |= FAN_MISC;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +MISC", from);
        }

      	if(app->config->checkKey("debug_internal", "true") && 
		(app->DEBUG_LVL & FAN_INTERNAL) == 0)
        {
                app->DEBUG_LVL |= FAN_INTERNAL;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +INTERNAL", from);
        }

  	if(app->config->checkKey("debug_socket", "true") && 
		(app->DEBUG_LVL & FAN_SOCKET) == 0)
        {
                app->DEBUG_LVL |= FAN_SOCKET;
                FAN_xlog(FAN_ERROR | FAN_INIT,"%s: DEBUG-LVL +SOCKET", from);
        }

        if(app->config->checkKey("log_facility", "db") && 
		(app->LOG_FACILITY & FAN_DB) == 0)
        {
                FAN_xlog(FAN_DEBUG | FAN_INIT,"%s: LOG-FACILITY: DB", from);
                app->LOG_FACILITY = FAN_DB;
        }

        if(app->config->checkKey("log_facility", "syslog") && 
		(app->LOG_FACILITY & FAN_SYSLOG) == 0)
        {
                FAN_xlog(FAN_DEBUG | FAN_INIT,"%s: LOG-Facility: SYSLOG", from);
                app->LOG_FACILITY = FAN_SYSLOG;
        }

        if(app->config->checkKey("log_facility", "console") && 
		(app->LOG_FACILITY & FAN_CONSOLE) == 0)
        {
                FAN_xlog(FAN_DEBUG | FAN_INIT, "%s: Log-Facility: CONSOLE", from);
                app->LOG_FACILITY = FAN_CONSOLE;
        }
	FAN_RETURN;
}

void FAN_upperCase(char *buffer)
{  
	while(*buffer != '\0')
	{
		if(*buffer >= 'a' && *buffer <= 'z')
		{ 
			*buffer += 'A' - 'a';
		}
		buffer++;
	}
}

int FAN_readall(char *line, int size, int FH)
{
	FAN_ENTER;
        char *p = line;
        int lsize = 0;

        lsize = read(FH, p, size);
	if(lsize >= 0)
	{
	  *(p+lsize) = '\0';
	}

        FAN_RETURN lsize;
}

int FAN_recvall(char *line, int size, int FH)
{
	FAN_ENTER;
    char *p = line;
    int lsize = 0;

	lsize = recv(FH, p, size, MSG_DONTWAIT);
	if(lsize >= 0)
	{
	  *(p+lsize) = '\0';
	}

    // if(lsize == -1)lsize = 0;

    FAN_RETURN lsize;
}



bool FAN_binaryrecv(int FH, char *templ, int size, unsigned char* buffer)
{
	FAN_ENTER;
    int len;

	unsigned char* pline = buffer;

    len=recv(FH, pline, size, MSG_WAITALL);

    if(len < size)
    {
        FAN_RETURN false;
    }

    pthread_t thread = pthread_self();
    if(thread > 0)
    {
      	FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);
	
		char *le = conf->config->getValue("endian");
		char *re = conf->config->getValue("remoteEndian");
		if(le != NULL && re != NULL && strcmp(le, re) != 0)
		{
			FAN_reverseByteOrder((unsigned char*)buffer, templ, true);
		}
	}

	FAN_RETURN true;
}

char *FAN_arecvlineC(int FH, bool file, int bufferSize, char* buffer)
{
	FAN_ENTER;
	int 	SIZE = bufferSize;
	int len = 0;
	int off = 0;

	while((len = recv(FH, (buffer+off), 1, 0)) == -1
	    || (buffer[off+len-1] != '\n' && len + off < SIZE - 2)) {
          if(len > 0)
	     off += len;
	}

	if(buffer[off] == '\n')
	{
		buffer[off+1] = '\0';
		char *total = NULL;
		asprintf(&total, "%s", buffer);
		FAN_RETURN total;
	}

	FAN_RETURN NULL;
}

char *FAN_arecvline(int FH, bool file, int bufferSize, char* buffer)
{
	FAN_ENTER;
	int 	SIZE = bufferSize;
	char    *line = buffer;
    char    *pline = line;

    char    *append = "%s%s";
    char    *total  = strdup(append);
	
	int size = SIZE;
    int len;
    int glen = 0;

    bool end = false;
    bool error = false;

	line[0] = '\0';

    while(!end)
    {
//		FAN_xlog(FAN_ERROR, "before readall");
        len=FAN_recvall(pline, size - 1, FH);
//		FAN_xlog(FAN_ERROR, "after readall (%d)", len);

		if(len == -1)
		{
			FAN_RETURN NULL;
		}
		pline += len;
		size -= len;
        glen += len;

//		FAN_xlog(FAN_ERROR, "glen: %d", glen);
        if(pline - line > SIZE-5)
        {
            char *p = total;
            asprintf(&total, total, line, append);
            free(p);
			size  = SIZE;
			pline = line;
			line[0] = '\0';
        }

        if(pline - line >= 5)
        {
            char *last = (pline-5);
            if(strstr(last, "\n") != NULL)
            {
                end = true;
				error = false;
            }
        }
    }
	if(pline != line)
	{
        char *p = total;
        asprintf(&total, total, line, append);
        free(p);
	}

    char *p = total;
	asprintf(&total, total, "", "");
	free(p);
//	free(line);

	if(glen > 0 && !error)
	{
		FAN_RETURN total;
	}
	else
	{
		if(total != NULL) free(total);
		FAN_RETURN NULL;
	}
}



char *FAN_areadline(int FH, bool file, int bufferSize, char* buffer)
{
	FAN_ENTER;
	int 	SIZE = bufferSize;
	char    *line = buffer;
    char    *pline = line;

    char    *append = "%s%s";
    char    *total  = strdup(append);
	
	int size = SIZE;
    int len;
    int glen = 0;

    bool end = false;
	bool error = false;

	line[0] = '\0';

        while(!end)
        {
//		FAN_xlog(FAN_ERROR, "before readall");
            len=FAN_readall(pline, size - 1, FH);
//		FAN_xlog(FAN_ERROR, "after readall (%d)", len);

		if(len == -1)
		{
			FAN_RETURN NULL;
		}
		pline += len;
		size -= len;
        glen += len;

//		FAN_xlog(FAN_ERROR, "glen: %d", glen);
        if(pline - line > SIZE-5)
        {
            char *p = total;
            asprintf(&total, total, line, append);
            free(p);
			size  = SIZE;
			pline = line;
			line[0] = '\0';
        }else if(len == 0)
        {
            end = true;
			error = true;
        }

        if(pline - line >= 5)
        {
            char *last = (pline-5);
            if(strstr(last, "\n") != NULL)
            {
                end = true;
	            error = false;
            }
        }
    }
	if(pline != line)
	{
        char *p = total;
        asprintf(&total, total, line, append);
        free(p);
	}

    char *p = total;
	asprintf(&total, total, "", "");
	free(p);
//	free(line);

	if(glen > 0 && !error)
	{
		FAN_RETURN total;
	}
	else
	{
		if(total != NULL) free(total);
		FAN_RETURN NULL;
	}
}

int FAN_swrite(int s,char *txt) {
//	FAN_xlog(FAN_DEBUG|FAN_NOCR|FAN_QUIET|FAN_SOCKET,txt);
        return FAN_swrite(s, (unsigned char *)txt, strlen(txt));
}

int FAN_swrite(int s,unsigned char *data, int size) {
	int len = 0;

	len = send(s,data,size, MSG_WAITALL);
	return len;
}

int FAN_clearCR(char *buf)
{
	FAN_ENTER;
	int i;
	int cc=strlen(buf);
        for(i=cc;i>0;i--)
                {
                if(buf[i] != '\r' && buf[i] != '\n' && buf[i] != '\0') break;
                buf[i]='\0';
                }               
	FAN_RETURN cc-i;
}                   

#ifdef __IRIX__
int asprintf( char** s, const char* f, ... )
  {
     //int params = 0;
     va_list argument;
     va_start(argument, f);

     char* buf;
     int buf_len = 100;
     int print_len;
     buf = (char*)malloc(buf_len);
     print_len = vsnprintf( buf, buf_len, f, argument );
     while (buf_len <= print_len+1) {
       buf_len *= 2;
       buf = (char*)realloc(buf, buf_len);
       print_len = vsnprintf( buf, buf_len, f, argument );
     }
     *s = (char*)realloc(buf, print_len+1);
     va_end(argument);
     
     return print_len; // is this right? --OM
}
#endif

int FAN_clearCCR(char *buf,char c)
{
	FAN_ENTER;
	int i,cc=strlen(buf);
        for(i=cc;i>0;i--)
                {
//		xlog(DEBUG|MISC,"%c : %c",buf[i],c);
                if(buf[i] != c && buf[i] != '\0') break;
                buf[i]='\0';
                }               
	FAN_RETURN cc-i;
}                    

void FAN_clearLastN(char *buf, int n)
{
	FAN_ENTER;
	char *pbuf;
	int len = strlen(buf); 
	
	pbuf = buf;
	FAN_xlog(FAN_DEBUG | FAN_INFO, pbuf);

	if(len >= n)
	{
		memset((void*)(buf+len-n), '\0', n); 			
	}
	
	FAN_xlog(FAN_DEBUG | FAN_INFO, pbuf);
	FAN_RETURN;
}

char *FAN_aRegularReplace(char *orig_buf, char *pre, char *post, FAN_Hash *hash)
{
	FAN_ENTER;
	char *buf;
	char *buf_base;
	char *_buf;
	char *_pre;
	char *_post;
	char *_ret;
	char *p;
	
	int status = FAN_BEFORE_KEY;

	buf_base = buf = strdup(orig_buf);

	_buf  = buf;
	_pre  = pre;
	_post = post;
	
	if(hash == NULL)
	{
		asprintf(&_ret, "%s", "%s%s%s");
	}else
	{
		asprintf(&_ret, "%s", "%s%s");
	}
	

	while(*buf != '\0')
	{
		switch(status)
		{
			case FAN_BEFORE_KEY :
				while(*buf != '\0' && *buf != *_pre)
				{
					buf++;
				}
				while(*buf != '\0' && *_pre != '\0' && *buf == *_pre)
				{
					buf++;
					_pre++;
				}
				if(*_pre == '\0' && buf != '\0')
				{
					*(buf-strlen(pre)) = '\0';
					
					if(hash == NULL)
					{
						p = _ret;
						asprintf(&_ret, _ret, _buf, post, "%s%s%s");
						free(p);
						status = FAN_BEFORE_KEY;
					}else
					{
						p = _ret;
						asprintf(&_ret, _ret, _buf, "%s%s");
						free(p);
						status = FAN_KEY;
					}

					_buf = buf;
				}
				_pre = pre;
				break;
			case FAN_KEY :
				while(*buf != '\0' && *buf != *_post)
				{
					buf++;
				}
				while(*buf != '\0' && *_post != '\0' && *buf == *_post)
				{
					buf++;
					_post++;
				}
				if(*_post == '\0' && buf != '\0')
				{
					*(buf-strlen(post)) = '\0';
					p = _ret;

					bool required = false;

					// FAN_xlog(FAN_ERROR, _buf);

					if(*(_buf) == '!')
					{
						_buf++;		
						required = true;
					}
					
					char *val = hash->getValue(_buf);

					if(val != NULL)
						asprintf(&_ret, _ret, val, "%s%s");
					else
					{
						if(!required)
							asprintf(&_ret, _ret, "", "%s%s");
						else
						{
							char *tmp = (char*)(_ret+strlen(_ret)-5) ;
							memcpy(tmp, "%s%s\0", 5);
							asprintf(&_ret, _ret, _buf, "%s%s");
							buf++;

						}
					}
						
					free(p);
					status = FAN_BEFORE_KEY;
					_buf = buf;
				}

				_post = post;
				break;
		}
	}

	p = _ret;
	if(hash == NULL)
	{
		asprintf(&_ret, _ret, _buf, "", "");
		
	}else
	{
		asprintf(&_ret, _ret, _buf, "");
	}
	free(p);
	free(buf_base);
	
	FAN_RETURN _ret;
}

int FAN_droproot(char *username)
{
	FAN_ENTER;
	int gid,uid,oid;
	struct passwd *pw = NULL;
	oid=getuid();
	pw = getpwnam( username );
	if (!pw) {
		FAN_xlog(FAN_DEBUG|FAN_INTERNAL,"User %s does not exist",username);
		FAN_RETURN -2;
	}else {
		uid=pw->pw_uid;
		gid=pw->pw_gid;	
		if ( 	initgroups(username, pw->pw_gid) != 0 || 
			setgid(pw->pw_gid) != 0 || setuid(pw->pw_uid) != 0 )
		{
			FAN_xlog(FAN_DEBUG|FAN_INTERNAL,"Couldn't change to %s uid=%d gid=%d\n", 
					username, pw->pw_uid, pw->pw_gid);
			FAN_RETURN -1;
		}
	}
	FAN_RETURN oid;
}

void FAN_dumpHex(char *ptr,int size)
{
	FAN_ENTER;
	int i,j;
	int mod=20;		// newline modulus;
	int lvl=FAN_DEBUG|FAN_MISC;
	char c;
	FAN_xlog(lvl|FAN_NOCR,"Hexdump of memory from 0x%x to 0x%x",ptr,(char*)(ptr+size));

	FAN_xlog(lvl,"Hexdump of memory from 0x%x to 0x%x",ptr,(char*)(ptr+size));
	for(i=0;i<size;i++)
		{
		if((i%mod) == 0) {
			if(i){
				FAN_xlog(lvl|FAN_NOCR|FAN_QUIET,"     ");
				for(j=20;j>0;j--) { 
					c=*(char*)(ptr+i-j);
//					xlog(DEBUG|MISC,"%x",(ptr+i-j));
					if(c < 32 || c > 126) c='.';
					FAN_xlog(lvl|FAN_NOCR|FAN_QUIET,"%c",c);
					}
			}
			FAN_xlog(lvl|FAN_NOCR|FAN_QUIET,"\n%x :",(char*)(ptr+i));
			}
		FAN_xlog(lvl|FAN_NOCR|FAN_QUIET," %.2x",*(ptr+i));
		}
	FAN_xlog(lvl|FAN_QUIET,"");
	FAN_RETURN;
}

char *FAN_aGetBuild()
{
	FAN_ENTER;
	char *ver=(FAN::app->config->getValue("version"));
	char *build = FAN_BuildNumber::aGetBuild();
	asprintf(&ver, "%s / FAN build: %s", FAN::app->config->getValue("version"), build);
	free(build);
 
        FAN_RETURN ver;
}

int FAN_getThreadId(void)
{
	FAN_ENTER;
	FAN_RETURN FAN__getThreadId();
}

int FAN__getThreadId(void)
{
	pthread_t p = pthread_self();
	return (int) p; // & 0x00000fff;
}



long FAN_generateWebCode(void)
{
	FAN_ENTER;
	time_t t;
	time(&t);
	int p = FAN_getThreadId();
	FAN_RETURN(p+t);
}

FAN_Connection *FAN_connect(char *server)
{
    FAN_ENTER;
    FAN *app = (FAN*)FAN_getApp();
    
    char *key="CONNRETRY";
    char *tmp=FAN::app->config->getValue(key);
    
    if(tmp == NULL)
    {
        tmp="1000";
        FAN_xlog(FAN_ERROR, "Can not find config value \"%s\", setting default \"%s\"",key,tmp);
    }
    int retry=atoi(tmp);
    
    key="CONNWAIT";
    tmp=FAN::app->config->getValue(key);
    
    if(tmp == NULL)
    {
        tmp="1";
        FAN_xlog(FAN_ERROR, "Can not find config value \"%s\", setting default \"%s\"",key,tmp);
    }
    int wait=atoi(tmp);
    
    asprintf(&key, "%sHOST", server);
    char *host=FAN::app->config->getValue(key);
    free(key);
    
    if(host == NULL)
    {
        host="127.0.0.1";
        FAN_xlog(FAN_ERROR, "Can not find config value \"%s\", setting default \"%s\"",key,host);
    }
    
    asprintf(&key, "%sPORT", server);
    char *port=FAN::app->config->getValue(key);
    free(key);
    
    if(port == NULL)
    {
        port="30003";
        FAN_xlog(FAN_ERROR, "Can not find config value \"%s\", setting default \"%s\"",key,port);
    }
    FAN_Connection *conn;
    
    for (int a=0; a < retry && (conn=app->connect( NULL, host, port)) == NULL; a++)
    {
	FAN_wait(wait,0);
    }
    FAN_RETURN conn;
}
