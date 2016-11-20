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



#ifndef _FAN_H
#define _FAN_H

#include "FANClasses.h"

#define ZAP(p) if(p) { delete p; p = NULL; }
#define ZAP_ARRAY(p) if(p) { delete[] p; p = NULL; }        
#define MZAP(p) if(p) { free(p); p = NULL; }

/**
 * 
 * @see FAN_Error::vlog
 */
#ifdef __GNUC__
#define FAN_xlog(a,b,c...) FAN_Error::zlog(a,__FUNCTION__, __LINE__, __FILE__ ,b,  ## c);
#else
void FAN_xlog(int, char*, ...);
#endif
/**
 * Logs error to the logging facility with trace.
 * 
 * @see FAN_Error::vlog
 */
#ifdef __GNUC__
#define FAN_err(b,c...) FAN_Error::zlog(FAN_ERROR,__FUNCTION__, __LINE__, __FILE__ ,b,  ## c);
#else
void FAN_err(char*, ...);
#endif
/**
 * Logs to the logging facility with trace and exits the program. 
 * 
 * @see FAN_Error::vlog
 */
#ifdef __GNUC__
#define FAN_plog(a,b,c...) FAN_Error::zlog(FAN_EXIT|a,__FUNCTION__, __LINE__, __FILE__ ,b,  ## c);
#else
void FAN_plog(int, char*, ...);
#endif
/**
 * Outputs a trace to the logging facility
 */
#ifdef __GNUC__
#define FAN__BACKTRACE FAN_Error::trace(__FUNCTION__, __LINE__, __FILE__,true)
#define FAN__TRACEBACK FAN_Error::trace(__FUNCTION__, __LINE__, __FILE__,false)
#else
#define FAN__BACKTRACE FAN_Error::trace("unknown", __LINE__, __FILE__,true)
#define FAN__TRACEBACK FAN_Error::trace("unknown", __LINE__, __FILE__,false)
#endif

#define FAN_BTRACE FAN__BACKTRACE

/**
 * Enable traceback
 */
#ifdef FAN_BT
	#define FAN_BACKTRACE FAN__BACKTRACE
	#define FAN_TRACEBACK FAN__TRACEBACK
#else 
	#define FAN_BACKTRACE
	#define FAN_TRACEBACK
#endif

/**
 * FAN function entrypoint for tracing.
 */
#define FAN_ENTER FAN_BACKTRACE
#ifdef FAN_TRACEBACK
	#define FAN_RETURN FAN_TRACEBACK;return
#else
/**
 * FAN function return for tracing.
 */
	#define FAN_RETURN return
#endif

#define FAN_OK          0

/**
 * @see FAN_xlog 
 */
#define FAN_INFO        0x1
/**
 * @see FAN_xlog
 */
#define FAN_DEBUG       0x2
/**
 * @see FAN_xlog
 */
#define FAN_WARN        0x4
/**
 * @see FAN_xlog
 */
#define FAN_ERROR       0x8

/**
 * @see FAN_xlog
 */
#define FAN_EXIT        0x10
/**
 * @see FAN_xlog
 */
#define FAN_SYSLOG      0x20
/**
 * @see FAN_xlog
 */
#define FAN_CONSOLE     0x40
/**
 * @see FAN_xlog
 */
#define FAN_DB          0x40
/**
 * @see FAN_xlog
 */
#define FAN_TRACE       0x80
/**
 * @see FAN_xlog
 */

/**
 * @see FAN_xlog
 */
#define FAN_HASH        0x400
/**
 * @see FAN_xlog
 */
#define FAN_INTERNAL    0x400
/**
 * @see FAN_xlog
 */
#define FAN_CONFIG      0x800
/**
 * @see FAN_xlog
 */
#define FAN_MISC        0x1000
/**
 * @see FAN_xlog
 */
#define FAN_SQL         0x2000
/**
 * @see FAN_xlog
 */
#define FAN_SOCKET      0x4000
/**
 * @see FAN_xlog
 */
#define FAN_INIT        0x8000

/**
 * @see FAN_xlog
 */
#define FAN_GREEN       0x10000
/**
 * @see FAN_xlog
 */
#define FAN_RED         0x20000
/**
 * @see FAN_xlog
 */
#define FAN_NOCR        0x40000 // Log without CR
/**
 * @see FAN_xlog
 */
#define FAN_QUIET       0x80000 // Log without TIME/SOURCE

/**
 * @see FAN_xlog
 */
#define FAN_ALL         FAN_HASH | FAN_CONFIG | FAN_SQL | FAN_SOCKET | FAN_INIT | FAN_MISC

/**
 * The FAN main application class. 
 */
class FAN
{
	/**
	 * Install default signal handlers  (NOT USED) 
	 */
	void setDefaultSignalHandlers (void);
	pthread_mutex_t mut;

	public:

	/**
	 * Reference to the daemon instance (FAN_ThreadedDaemon) or NULL if client
	 */
	FAN_ThreadedDaemon *theDaemon;

	/**
	 * the one and only application 
	 */
	static FAN *app;
	/**
	 * Is a unix socket server ? (NOT IMPLEMENTED)
	 */
	bool isFileSocket;

	/**
	 * Current debug level 
	 *
	 * @see FAN_DEBUG
	 * @see FAN_INFO
	 * @see FAN_WARN 
	 * @see FAN_ERROR
	 */
	int DEBUG_LVL;
	/** 
	 * Current log level 
	 *
	 * @see FAN_HASH
	 * @see FAN_CONFIG
	 * @see FAN_SQL
	 * @see FAN_SOCKET
	 * @see FAN_INIT
	 * @see FAN_MISC
	 */ 
	int LOG_LVL;
	/** 
	 * Current logging facility 
	 *
	 * @see FAN_CONSOLE
	 * @see FAN_SYSLOG
	 * @see FAN_DB
	 * */
	int LOG_FACILITY;

	/**
	 * Do not log debugging messages 
	 */
	int DEBUG_IGNORE;
	
	/**
	 * Tree for thread configurations 
	 */
	pthread_key_t threadFAN;
	/**
	 * last error number 
	 */
	int errorNo;

	/**
	 * Holds all status relevant global variables
	 */
	class SystemStatus {
		public:
		SystemStatus()
		{
			this->connectionCounter=0;
		}
		/**
		 * Start time of the server 
		 */
		time_t startTime;
		/** 
		 * Total number of connections 
		 */ 
		int connectionCounter;
		/**
		 * Total number of valid authentications 
		 */
		int authenticatedConnections;
		/** 
		 * Total number of failed authentications 
		 */
		int authenticationFailed;
	};

	/** 
	 * The current server status 
	 */
	SystemStatus sysStatus;

	/** 
	 * The server socket 
	 */
	int acceptSocket;
	/** 
	 * Is threaded ? 
	 */
	int threaded;

	/**
	 * The connection to a system directory (Config-Server) 
	 */
	FAN_Connection *sddConn;

	/**
	 * Is clear text 
	 */
	int clearText;

	/**
	 * The current namespace 
	 */
	char *use;

	/** 
	 * A hash with the return values of the last rpc 
	 */
	FAN_Hash *returnHash;
	/** 
	 * A hash with the client/server configuration 
	 */
	FAN_Hash *config;

	/**
	 * Connects to SDD
	 * The parameters are taken from the configfile.
	 *
	 * @returns true if succeed
	 */
	bool connectSDD();
	/**
	 * Connect to SDD on [host]:[port]
	 * Use namespace [service]
	 *
	 * @param service The default namespace
	 * @param host The SDD hostname / ip
	 * @param port The SDD tcp-port
	 *
	 * @returns FAN_Connection instance
	 */
	FAN_Connection *connect(char *service, char *host, char *port);

	void installIntHandler();
	void lockConfig();
	void unlockConfig();

	/**
	 * Constructor
	 *
	 * Generates a new thread instance
	 *
	 * @param fan the parent application
	 */
	FAN(FAN *fan);
	/**
	 * Constructor 
	 *
	 * Generates a new application instance
	 *
	 * @param file Path to config file
	 * @param connect Connect to SDD ?
	 */
	FAN(char * file,bool connect);
	/**
	 * Destructor
	 */
	~FAN();
};

/** \example Client.cpp
 * This is an example of how to use the Application class.
 */

#endif
