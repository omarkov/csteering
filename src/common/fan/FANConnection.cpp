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
#include <sys/socket.h>

FAN_Connection::FAN_Connection(char *service, char *host, int port) {
    if(service != NULL)
        this->service=strdup(service);
    else
        this->service=NULL;
    this->isFileSocket=false;
    this->host = strdup(host);
    this->port = port;
    this->connSock = 0;
    this->connected = false;
    returnHash = new FAN_Hash();
}

FAN_Connection::FAN_Connection(char *service){
    if(service != NULL)
        this->service=strdup(service);
    else
        this->service=NULL;
    this->connSock = 0;
    this->isFileSocket=false;
    returnHash = new FAN_Hash();
}

FAN_Connection::~FAN_Connection(){
    FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "cleanup CONNECTION");

    if(isConnected())
	disconnect();
    if(service != NULL)
        free(service);
    if(host != NULL)
        free(host);
    if(returnHash != NULL)
	delete returnHash;
}

bool FAN_Connection::connect()
{
    FAN_ENTER;
    FAN_RETURN connect(host,port);
}

bool FAN_Connection::connect(char *cHost,int cPort)
{
    FAN_ENTER;
    
    this->host = cHost;
    this->port = cPort;
    
    FAN_xlog(FAN_DEBUG|FAN_SOCKET," to %s:%i", cHost,cPort);
    connSock = FAN_getClientSocket(cHost, cPort);
    
    // returnHash = new FAN_Hash();
    
    if(connSock > 0)
    {
        rpc("sys::endian", 1, FAN::app->config->getValue("endian"));

        if(service != NULL)
        {
            rpc("use", 1, service);
        }
        FAN_RETURN true;
    }
    else
    {
        FAN_RETURN false;
    }
}

bool FAN_Connection::isConnected()
{
        FAN_ENTER;
	if(connSock != 0)
	{
		if(rpc("sys::ping"))
		{
		   FAN_RETURN true;
		}else
		{
		   FAN_RETURN false;
		}
	}
	else
	{
		FAN_RETURN false;
	}
}

bool FAN_Connection::disconnect()
{
        FAN_ENTER;
        if(connSock > 0)
        {
                if((FAN::app->errorNo = close(connSock)) == 0)
		{
                        FAN_RETURN true;
		}

                connSock = 0;
        }
        FAN_RETURN true;
}

char *FAN_Connection::getReturnKey(char *key)
{
    FAN_ENTER;
    FAN_RETURN returnHash->getValue(key);
}

bool FAN_Connection::rpc(char *name)
{
    FAN_ENTER;
    bool ret = vrpc(name, 0, 0);
    FAN_RETURN (returnHash->checkKey("RETURN","TRUE") && ret); 
}

bool FAN_Connection::rpc(char *name, int count, ...)
{
    FAN_ENTER;
    va_list argument;
    va_start(argument, count);
    int ret = vrpc(name, count, argument);
    va_end(argument);
   
    FAN_RETURN (returnHash->checkKey("RETURN","TRUE") && ret); 
}

bool FAN_Connection::rpc(FAN_Hash *hash, char *name, int count, ...)
{
    FAN_ENTER;
    
    va_list argument;
    va_start(argument, count);
    FAN_RETURN vrpc(hash, name, count, argument);
    va_end(argument);
}

bool FAN_Connection::vrpc(FAN_Hash *hash, char *name, int count, va_list argument)
{
    FAN_ENTER;
    if(connSock > 0)
    {
        FAN_RETURN FAN_vrpc(hash, connSock, name, count, argument);
    }else
    {
	FAN_RETURN false;
    }
}

bool FAN_Connection::vrpc(char *name, int count, va_list argument)
{
    FAN_ENTER;
    if(returnHash != NULL)
    {
        returnHash->clear();
        
        FAN_RETURN vrpc(returnHash, name, count, argument);
    }
    FAN_RETURN false;
}

bool FAN_Connection::stopBinaryPush()
{
    if(connSock > 0)
    {
    	FAN_swrite(connSock, "\n");
	int len = 0;
	char buf[2];
	len = recv(connSock, buf, 1, MSG_WAITALL);
	if(len < 0)
	{
		return false;
	}
    }
    return true;
}

bool FAN_Connection::startBinaryPush(char *fkt)
{
    return rpc("binarypush", 1, fkt);
}

bool FAN_Connection::binaryPush(char *fmt, ...)
{
    FAN_ENTER;
    va_list argument;
    va_start(argument, fmt);
    bool ret = vbinaryPush(fmt, argument);
    va_end(argument);
    
    FAN_RETURN (returnHash->checkKey("RETURN","TRUE") && ret); 
}

bool FAN_Connection::rpc(char *name, char *fmt, ...)
{
    FAN_ENTER;
    va_list argument;
    va_start(argument, fmt);
    bool ret = vrpc(name, fmt, argument);
    va_end(argument);
    
    FAN_RETURN ret;
}

bool FAN_Connection::rpc(FAN_Hash *hash, char *name, char* fmt, ...)
{
    FAN_ENTER;
    
    va_list argument;
    va_start(argument, fmt);
    FAN_RETURN vrpc(hash, name, fmt, argument);
    va_end(argument);
}

bool FAN_Connection::vbinaryPush(char *fmt, va_list argument)
{
    FAN_ENTER;
    if(connSock > 0)
    {
        int ret = FAN_vbinaryPush(connSock, fmt, argument);
	if(ret < 0)
	{
		connSock = 0;
		FAN_RETURN false;
	}
	else
	{
		FAN_RETURN ret;
	}
    }
    else 
    {
        FAN_RETURN false;
    }
}



bool FAN_Connection::vrpc(FAN_Hash *hash, char *name, char *fmt, va_list argument)
{
    FAN_ENTER;
    if(connSock > 0)
    {
        int ret = FAN_vrpc(hash, connSock, name, fmt, argument);
	if(ret < 0)
	{
		connSock = 0;
		FAN_RETURN false;
	}
	else
	{
		FAN_RETURN ret;
	}
    }
    else 
    {
        FAN_RETURN false;
    }
}

bool FAN_Connection::vrpc(char *name, char *fmt, va_list argument)
{
    FAN_ENTER;
    if(returnHash != NULL)
    {
        returnHash->clear();
        
        FAN_RETURN vrpc(returnHash, name, fmt, argument);
    }
    FAN_RETURN false;
}
