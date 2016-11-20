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
#include <sys/types.h>   
#include "FANClasses.h"

#define FAN_BEFORE_KEY       0x0
#define FAN_KEY              0x1
#define FAN_AFTER_KEY        0x2

FAN *FAN::app = NULL;

void intHandler(int)
{
/*	if(FAN::app != NULL && FAN::app->theDaemon != NULL)
	   FAN::app->theDaemon->shutdownDaemon();*/
	FAN_xlog(FAN_ERROR, "SIGINT captured. Try to shutdown...");
	FAN_sysHalt();
}

void FAN::setDefaultSignalHandlers(void)
{
    // might fail on linux
    signal(SIGCHLD,SIG_IGN);

    signal(SIGPIPE,SIG_IGN);
    signal(SIGHUP ,SIG_IGN);
    signal(SIGALRM,SIG_IGN);
}

void FAN::installIntHandler()
{
#ifndef __IRIX__
    signal(SIGINT,&intHandler);
    signal(SIGTERM,&intHandler);
#endif
}

void my_destructor(void *data)
{
    data = NULL;
}

FAN::FAN(FAN *fan)
{
    threaded 	= fan->threaded;
    clearText 	= fan->clearText;
    theDaemon = NULL;

    setDefaultSignalHandlers();
    use = NULL;

    sddConn = NULL;

    errorNo 	= 0;
    returnHash 	= NULL;

    LOG_LVL      	= fan->LOG_LVL;
    DEBUG_LVL    	= fan->DEBUG_LVL;
    LOG_FACILITY 	= fan->LOG_FACILITY;
    DEBUG_IGNORE 	= fan->DEBUG_IGNORE;

    config 		= new FAN_Hash();
    fan->lockConfig();
    fan->config->copyValues(config);
    fan->unlockConfig();

}

FAN::FAN(char *file,bool connect)
{
    threaded = 0;    // Threaded write here

    use = NULL;
    theDaemon = NULL;

    pthread_mutex_init(&mut, NULL);
    pthread_key_create(&threadFAN, my_destructor);

    time(&sysStatus.startTime);

    FAN::app = this;

    sddConn = NULL;

    setDefaultSignalHandlers();

    errorNo   = 0;

    returnHash = NULL;

    clearText = 0;

    LOG_LVL      = FAN_ERROR;
    LOG_FACILITY = FAN_CONSOLE;
    DEBUG_IGNORE = FAN_HASH;
    DEBUG_LVL    = 0;

    config = new FAN_Hash();

    if(config->readFromFile(file))
    {
    	FAN_xlog(FAN_DEBUG|FAN_INIT,"Read config file %s",file);
        FAN_setLogs("Client");
    }

    if(connect){
        FAN_xlog(FAN_DEBUG|FAN_INIT,"Read config from sdd");
        if(config->checkKey("sddconnect","true") && 
           config->getValue("sddhost") != NULL && 
           config->getValue("sddport") != NULL)
        {
            sddConn = new FAN_Connection("sdd",
                     config->getValue("sddhost"),
                atoi(config->getValue("sddport")));
            
            
            if(sddConn->connect()){
                sddConn->rpc(config,"readConf",0);
            } else
            {
            	FAN_xlog(FAN_DEBUG|FAN_INIT,"Could not connect.");
            	FAN_RETURN;
            }
            FAN_setLogs("Server");
            
            if(config->readFromFile(file))
            {
            	FAN_xlog(FAN_DEBUG|FAN_INIT,"Read config file %s",file);
            	FAN_setLogs("Client");
            }
        }	
    }	
}

void FAN::lockConfig()
{
    pthread_mutex_lock(&mut);
}

void FAN::unlockConfig()
{
    pthread_mutex_unlock(&mut);
}

FAN_Connection *FAN::connect(char *service, char *host, char *port)
{
    FAN_Connection *conn = new FAN_Connection(service, host, atoi(port));
    
    if(conn->connect())
    {
        FAN_RETURN conn;
    }else
    {
        FAN_xlog(FAN_ERROR, "Connection failed");
        delete conn;
        FAN_RETURN NULL;
    }
}

FAN::~FAN()
{
    FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "cleanup FAN");

    if(sddConn != NULL)
        delete sddConn;

    if(config != NULL)
        delete config;

    if(returnHash != NULL)
        delete returnHash;

/*    if(theDaemon != NULL)
    	theDaemon->shutdownDaemon();*/


    if(FAN::app == this) FAN::app = NULL;
#ifdef _THREADED_SAFE
    pthread_mutex_destroy(&mut);
#endif
}
