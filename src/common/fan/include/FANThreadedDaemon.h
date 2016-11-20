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



#ifndef _FAN_THREADED_DAEMON
#define _FAN_THREADED_DAEMON

#define FAN_BEFORE_CMD       0x0
#define FAN_CMD              0x1
#define FAN_AFTER_CMD        0x2
#define FAN_BEFORE_PARAM     0x4
#define FAN_PARAM            0x8
#define FAN_EXT_PARAM        0x10
#define FAN_QUOTE            0x20
#define FAN_AFTER_PARAM      0x40
#define FAN_READLINE_MAXLEN  1024

/**
 * Implements a threaded daemon with remote procedure call and message routing functionality
 */
class FAN_ThreadedDaemon
{
	/**
	 * Path to local config file
	 */
	char *confFile;
	/**
	 * Interupt (NOT IMPLEMENTED)
	 */
	int  interupt;
	/**
	 * the server socket
	 */
	int  acceptSocket;

	/**
	 * NOT IMPLEMENTED
	 */
	char *unixSocket;
	/**
	 * Is an unix socket ? (NOT IMPLEMENTED)
	 */
	bool isFileSocket;

	/**
	 * Shutdown in progress ?
	 */
	bool bShutdown;

public:
	/**
	 * Confighash key of the hostname of the sdd server
	 */
	char *confHost;
	/**
	 * Confighash key of the tcp-port of the sdd server
	 */
	char *confPort;


	/**
	 * Initializes a threaded daemon.
	 *
	 * @param cfgfile Local config file
	 * @param chost Confighash key of the hostname of the sdd server
	 * @param cport Confighash key of the tcp-port of the sdd server
	 */
	FAN_ThreadedDaemon(char *cfgfile, char *chost, char *cport);
	/**
	 * Initializes a threaded daemon with unix socket (NOT IMPLEMENTED)
	 *
	 * @param cfgfile Local config file
	 * @param unixSocketFile Path to unix socket
	 */
	FAN_ThreadedDaemon(char *cfgfile, char *unixSocketFile);
	/**
	 * Destructor
	 */
	~FAN_ThreadedDaemon();
	/**
	 * Bind the daemon and wait for incoming calls.
	 */
	bool bindDaemon();
	/**
	 * Shut down the daemon.
	 */
	void shutdownDaemon();
	/**
	 * Is the daemon running ? 
	 */
	bool running();
	/**
	 * Check server status.
	 *
	 * @return true if the server is binded.
	 */
	bool binded();
	/**
	 * Bind the daemon and wait for incoming calls.
	 * Spawn a thread and call the [master] function.
	 *
	 * @param master A threaded function
	 */
	bool bindDaemon(void*(*master)(void*));
	/**
	 * Bind the daemon and wait for incoming calls.
	 * Spawn a thread and call the [master] function.
	 * Spawn [count] threads and call the [worker] function in each thread.
	 *
	 * @param master A threaded function
	 * @param worker A threaded worker function
	 * @param count Number of worker threads to spawn 
	 */
	bool bindDaemon(void*(*master)(void*), void*(*worker)(void*), int count);
};

/**
 * Suspend the current thread for [s] seconds and [ns] nano-seconds.
 *
 * @param s Seconds to suspend
 * @param ns Nano-seconds to suspend
 */
int FAN_wait(long s, long ns);

/**
 * Initialize a message router for master message callbacks.
 *
 * @return A master message router
 */
FAN_Hash *FAN_initMasterHandler();
/**
 * Initialize a message router for worker message callbacks.
 *
 * @return A worker message router
 *
 * @see FAN_registerHandler
 * @see FAN_sendMessage
 * @see FAN_postMessage
 * @see FAN_initMessages
 * @see FAN_cleanupMessages
 */
FAN_Hash *FAN_initWorkerHandler();
/**
 * Start [count] worker threads.
 * 
 * @param reg The worker message router
 * @param count Number of threads
 *
 * @return The identifier hash
 *
 * @see FAN_registerHandler
 * @see FAN_startWorkers
 * @see FAN_sendMessage
 * @see FAN_postMessage
 * @see FAN_initMessages
 * @see FAN_cleanupMessages
 */
void FAN_startWorkers(FAN_Hash *reg, int count);

/**
 * Register a message handler.
 *
 * @param reg Message router
 * @param fkt The message type
 * @param handler Pointer to callback function
 *
 * @see FAN_initMasterHandler
 * @see FAN_initWorkerHandler
 * @see FAN_startWorkers
 * @see FAN_sendMessage
 * @see FAN_postMessage
 * @see FAN_initMessages
 * @see FAN_cleanupMessages
 */
void FAN_registerHandler(FAN_Hash *reg, char* fkt, void*(*handler)(FAN_Hash*, void*));

/**
 * Register a worker function. (Internal)
 */
bool FAN_registerWorkers(void*(*worker)(void*), int count, FAN_Hash*);
/**
 * Register a master function. (Internal)
 */
bool FAN_registerMaster(void*(*master)(void*), void *p);

/**
 * Register a worker function. 
 * 
 * @param worker Pointer to worker function
 * @param count Number of workers to spawn
 */
bool FAN_registerWorkers(void*(*worker)(void*), int count);
/**
 * Register a master function. 
 * 
 * @param master Pointer to master function
 */
bool FAN_registerMaster(void*(*master)(void*));

/**
 * Register a cleanup function. 
 * This function is called after a socket connection breaks.
 * 
 * @param cleanup Pointer to cleanup function
 */
void FAN_registerCleanup(void(*cleanup)(int));
/**
 * A default cleanup function.
 */
void FAN_defaultCleanup(int);
/**
 * The one and only RPC dispatcher
 */
void *FAN_dispatch(void *x);
/**
 * Generates a tcp server socket.
 *
 * @param hostname Binding interface (IP/hostname)
 * @param port Binding port
 */
int FAN_getBindSocket(char *hostname, int port);
/**
 * Generates a unix file socket server (NOT IMPLEMENTED).
 * 
 * @param listensock Path to unix socket file
 */
int FAN_getBindFileSocket(char *listensock);
/**
 * Parses the rpc command.
 *
 * @param line The command line received via server socket
 * @param cmd_pointer Pointer to command name
 * @param pparams Pointer to parameter list
 */
void FAN_parseCmd(char *line, char **cmd_pointer, char ***pparams);

#endif

/** \example server.cpp
 * This is an example of how to use the FAN_ThreadedDaemon class.
 */

