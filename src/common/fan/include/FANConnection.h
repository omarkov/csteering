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



#ifndef _FAN_CONNECTION
#define _FAN_CONNECTION

#include "FANClasses.h"

/**
 * Implementation of a socket connection for remote procedure calls. 
 */
class FAN_Connection{

        int connSock;
        bool connected;
        int port;
        char* host;
        char* service;
        char* sessionId;

public:
	bool isFileSocket;
        FAN_Hash *returnHash;

        /**
	 * Gets the corresponding value of a given key from the #returnHash of the RPC.
	 *
	 * @param key Key
	 * @returns a reference to the corresponding value of the key
	 */
	char* getReturnKey(char* key);

	/**
	 * Connects to the specified host at the specfied port to open an RPC connection.
	 *
	 * @param host Host
	 * @param port Port
	 * @returns True if successful and False otherwise
	 */
	bool connect(char* host, int port);
	/**
	 * Connects to the given host at the given port to open an RPC connection.
	 *
	 * @returns True if successful and False otherwise
	 * @see connect(char* host, int port), isConnected, disconnect
	 */
	bool connect();
	/**
	 * Checks for successful establishment of the socket connection.
	 *
	 * @returns True if a socket connection has been established and False otherwise
	 */
	bool isConnected();
	/**
	 * Closes the open socket connection.
	 *
	 * @returns True if successful and False otherwise
	 */
	bool disconnect();
	/**
	 * Clears the #returnHash, calls the remote procedure with the given name and arguments
	 * from the vector and fills the #returnHash with the results.
	 *
	 * @param name Name of remote procedure
	 * @param count Number of RPC arguments
	 * @param argument Argument vector
	 * @returns True if successful and False otherwise
	 * @see vrpc(FAN_Hash *hash, char *name, int count, va_list argument)
	 */
	bool vrpc(char *name, int count, va_list argument);
	/**
	 * Clears the #returnHash, calls the remote procedure with the given name and fills the 
	 * #returnHash with the results.
	 *
	 * @param name Name of remote procedure
	 * @returns True if successful and False otherwise 
	 * @see vrpc(char *name, int count, va_list argument) 
	 */
	bool rpc(char *name);
	/**
	 * Clears the #returnHash, calls the remote procedure with the given name and a variable 
	 * number of arguments and fills the #returnHash with the results.
	 *
	 * @param name Name of remote procedure
	 * @param count Number of passed parameters to be used as RPC arguments
	 * @returns True if successful and False otherwise 
	 * @see vrpc(char *name, int count, va_list argument)
	 */
	bool rpc(char *name, int count, ...);
	/**
	 * Calls the remote procedure with the given name and arguments from the vector 
	 * and inserts the results into the supplied hash.
	 *
	 * @param hash Result hash
	 * @param name Name of remote procedure
	 * @param count Number of RPC arguments
	 * @param argument Argument vector
	 * @returns True if successful and False otherwise 
	 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, int params, va_list argument)
	 */
	bool vrpc(FAN_Hash *hash, char *name, int count, va_list argument);
	/**
	 * Calls the remote procedure with the given name and a variable number of arguments
	 * and inserts the results into the supplied hash.
	 *
	 * @param hash Result hash
	 * @param name Name of remote procedure
	 * @param count Number of passed parameters to be used as RPC arguments
	 * @returns True if successful and False otherwise
	 * @see vrpc(FAN_Hash *hash, char *name, int count, va_list argument)
	 */
	bool rpc(FAN_Hash *hash, char *name, int count, ...);

	/**
	 * Clears the #returnHash, calls the remote procedure with the given name, binary data 
	 * format template, and arguments from the vector and fills the #returnHash with the results. 
	 *
	 * @param name Name of remote procedure
	 * @param fmt Binary data format template
	 * @param argument Argument vector
	 * @returns True if successful and False otherwise
	 * @see vrpc(FAN_Hash *hash, char *name, char *fmt, va_list argument) 
	 */
	bool vrpc(char *name, char *fmt, va_list argument);
	bool vbinaryPush(char *fmt, va_list argument);
	/**
	 * Clears the #returnHash, calls the remote procedure with the given name, binary data format
	 * template, and a variable number of arguments and fills the #returnHash with the results.
	 *
	 * @param name Name of remote procedure
	 * @param fmt Binary data format template
	 * @returns True if successful and False otherwise
	 * @see vrpc(char *name, char *fmt, va_list argument)
	 */
	bool rpc(char *name, char *fmt, ...);
	bool startBinaryPush(char *fkt);
	bool stopBinaryPush();
	bool binaryPush(char *fmt, ...);
	/**
	 * Calls the remote procedure with the given name, binary data format template, and arguments
	 * from the vector and inserts the results into the supplied hash.
	 *
	 * @param hash Result hash
	 * @param name Name of remote procedure
	 * @param fmt Binary data format template
	 * @param argument Argument vector
	 * @returns True if successful and False otherwise
	 * @see FAN_vrpc(FAN_Hash *hash, int sd, char *fkt, char *fmt, va_list argument)
	 */
	bool vrpc(FAN_Hash *hash, char *name, char *fmt, va_list argument);
	/**
	 * Calls the remote procedure with the given name, binary data format template, and a variable
	 * number of arguments and inserts the results into the supplied hash.
	 *
	 * @param hash Result hash
	 * @param name Name of remote procedure
	 * @param fmt Binary data format template
	 * @returns True if successful and False otherwise
	 * @see vrpc(FAN_Hash *hash, char *name, char *fmt, va_list argument)
	 */
	bool rpc(FAN_Hash *hash, char *name, char *fmt, ...);

	/**
	 * Destructor.
	 */
	~FAN_Connection();
	/**
	 * Constructor.
	 *
	 * @param service RPC namespace
	 */
	FAN_Connection(char *service);
	/**
	 * Constructor.
	 *
	 * @param service RPC namespace
	 * @param host Host
	 * @param port Port
	 */
	FAN_Connection(char *service, char* host, int port);
};

#endif
