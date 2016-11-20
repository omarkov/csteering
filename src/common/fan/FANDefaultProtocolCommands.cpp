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


// Generic thread callback functions

#ifndef __IRIX__
using namespace std;
#endif
#include "FANClasses.h"

#if !(defined(FREEBSD) || defined(__DARWIN_OSX__))
#include <crypt.h>
#endif

#define APPEND "%s%s"
#define END " "

time_t FAN_start;
FAN_Hash *FAN_cmd;

void FAN_init()
{
    FAN_ENTER;
    FAN_cmd = new FAN_Hash();
    FAN_start=time(0);

//          EXT NAME       HANDLER                   PUB    INIT
    FAN_createDomain("grid", false);

    FAN_createDomain("sdd", false);
    FAN_loadCmd("sdd::init", &FAN_cmdSddInit, true, false);
    FAN_loadCmd("sdd::readConf", &FAN_cmdSddReadConf, true, false);

    FAN_createDomain("sys", true);
    FAN_loadCmd("sys::status", &FAN_cmdSysStatus, true, true);
    FAN_loadCmd("sys::setBufferSize", &FAN_cmdSysSetBufferSize, true, true);
    FAN_loadCmd("sys::endian", &FAN_cmdSysEndian, true, true);
    FAN_loadCmd("sys::uptime", &FAN_cmdSysUptime, true, true);
    FAN_loadCmd("sys::UnloadCmd", &FAN_cmdSysUnloadCmd, true, true);
    FAN_loadCmd("sys::LoadCmd", &FAN_cmdSysLoadCmd, true, true);
    FAN_loadCmd("sys::halt", &FAN_cmdSysHalt, true, true);
    FAN_loadCmd("sys::ping", &FAN_cmdSysPing, true, true);

    FAN_loadCmd("exit", &FAN_cmdExit, true, true);
    FAN_loadCmd("use", &FAN_cmdUse, true, true);
    FAN_loadCmd("help", &FAN_cmdHelp, true, true);
    FAN_loadCmd("cleartext", &FAN_cmdClearText, true, true);
    FAN_loadCmd("nocleartext", &FAN_cmdNoClearText, true, true);
    FAN_RETURN;
}

void FAN_createDomain(char *domain, bool init)
{
	FAN_ENTER;
	char *text;
	asprintf(&text, "%s::use", domain);
	FAN_cmd->insertPointer(text,   (void*) new FAN_ProtocolCommand (&FAN_cmdUse          , true , init ));
	free(text);
	asprintf(&text, "%s::help", domain);
	FAN_cmd->insertPointer(text, (void*) new FAN_ProtocolCommand (&FAN_cmdHelp         , true , init ));
	free(text);
	FAN_RETURN;
}

void FAN_loadCmd(char *cmd, void (*fkt)(FAN_Hash*, char**),bool anonym, bool available)
{
	FAN_ENTER;
	FAN_cmd->insertPointer(cmd,(void*) new FAN_ProtocolCommand (fkt, anonym, available));
	FAN_RETURN;
}


void FAN_loadExtCmd(char *cmd, void (*fkt)(FAN_Hash*, char**),bool anonym, bool available)
{
	FAN_ENTER;
	FAN_cmd->insertPointer(cmd,(void*) new FAN_ProtocolCommand (fkt, anonym, available));
	FAN_cmdSysLoadCmd(cmd);
	FAN_RETURN;
}

void FAN_loadCommands(FAN_Hash *hash)
{
	FAN_ENTER;
	FAN_xlog(FAN_ERROR, "Init Commands");

	char *name = NULL;
	FAN_ProtocolCommand *pval;
	int n = 1;
	char *val;

	asprintf(&name, "cmdinit1");

	while((val = hash->getValue(name)) != NULL)
	{
		pval = (FAN_ProtocolCommand*)FAN_cmd->getPointer(val);
		if(pval != NULL)
		{
			FAN_xlog(FAN_ERROR, "Init Command: %s", val);
			pval->fkt(NULL, NULL);
		}
		free(name);
		asprintf(&name, "cmdinit%d", ++n);
	}
	FAN_xlog(FAN_ERROR, "free", name);
	free(name);

	asprintf(&name, "cmd1");
	n = 1;

	while((val = hash->getValue(name)) != NULL)
	{
		FAN_xlog(FAN_ERROR, "Load Command: %s", name);
		FAN_cmdSysLoadCmd(val);
		free(name);
		asprintf(&name, "cmd%d", ++n);
	}
	free(name);
	FAN_RETURN;
}

void FAN_final()
{
	FAN_ENTER;
	delete FAN_cmd;
	FAN_RETURN;
}

bool FAN_cmdSysLoadCmd(char *name)
{
	FAN_ENTER;
	if(name != NULL)
	{
		FAN_ProtocolCommand *p = (FAN_ProtocolCommand*)FAN_cmd->getPointer(name);
		if(p != NULL)
		{
			p->available = true;
			FAN_RETURN true;
		}
	}
	FAN_RETURN false;
}

bool FAN_cmdSysUnloadCmd(char *name)
{
	FAN_ENTER;
	if(name != NULL)
	{
		FAN_ProtocolCommand *p = (FAN_ProtocolCommand*)FAN_cmd->getPointer(name);
		if(p != NULL)
		{
			p->available = false;
			FAN_RETURN true;
		}
	}
	FAN_RETURN false;
}


void FAN_cmdSysHalt(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
	if(FAN_sysHalt())
	{
		ret->insert("RETURN", "true");
		ret->insert("RETURNMSG", "shuting down server....");
	}else
	{
		ret->insert("RETURN", "false");
		ret->insert("RETURNMSG", "shuting down failed");
	}
	FAN_RETURN;
}

void FAN_cmdSysPing(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
	ret->insert("RETURN", "true");
	ret->insert("RETURNMSG", "PONG");
	FAN_RETURN;
}

bool FAN_sysHalt()
{
	FAN_ENTER;
	if(FAN::app != NULL && FAN::app->theDaemon != NULL)
	{
		char *hostname = FAN::app->config->getValue(FAN::app->theDaemon->confHost);
		char *port     = FAN::app->config->getValue(FAN::app->theDaemon->confPort);

		FAN::app->theDaemon->shutdownDaemon();

		FAN_Connection *c = new FAN_Connection(NULL, hostname, atoi(port));
		c->connect();
		delete c;

		FAN_RETURN true;
	}else
	{
		FAN_RETURN false;
	}
}

void FAN_cmdSysLoadCmd(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
	char *name = NULL;

        FAN_aGetParam(params, &name, 0);

	if(FAN_cmdSysLoadCmd(name))	
	{
		ret->insert("RETURN", "TRUE");
		ret->insert("RETURNMSG", "Cmd successfully loaded");
	}else
	{
		ret->insert("RETURN", "FALSE");
		ret->insert("RETURNMSG", "Loading of cmd failed");
	}

	free(name);
	FAN_RETURN;
}

void FAN_cmdUse(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        char *name = NULL;

        FAN_aGetParam(params, &name, 0);

	if(name != NULL)
	{
	        FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

	        if(conf != NULL)
	        {
			if(conf->use != NULL)
			{
				free(conf->use);
			}

			conf->use=name;
        	        ret->insert("RETURN", "TRUE");
        	        ret->insert("RETURNMSG", "success");
        	}else
        	{
        	        char *msg;
        	        asprintf(&msg, "threadFAN does not exist !");
        	        FAN_xlog(FAN_ERROR, msg);
        	        ret->insert("RETURN", "FALSE");
        	        ret->insert("RETURNMSG", msg);
                	free(msg);
			free(name);
		}
        }else
	{
	        FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

		if(conf->use != NULL)
		{
			free(conf->use);
		}
		conf->use = NULL;

       	        ret->insert("RETURN", "TRUE");
       	        ret->insert("RETURNMSG", "success");
	}
	FAN_RETURN;
}

void FAN_cmdSysUnloadCmd(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        char *name = NULL;

        FAN_aGetParam(params, &name, 0);

        if(FAN_cmdSysUnloadCmd(name))
        {
                ret->insert("RETURN", "TRUE");
                ret->insert("RETURNMSG", "Cmd successfully unloaded");
        }else
        {
                ret->insert("RETURN", "FALSE");
                ret->insert("RETURNMSG", "Unloading of cmd failed");
        }

        free(name);
	FAN_RETURN;
}

void FAN_cmdGridInit(FAN_Hash *ret, char **params)
{	
	FAN_ENTER;
	FAN_cmdGridInit();
	FAN_RETURN;
}

void FAN_cmdGridInit()
{	
	FAN_ENTER;
	FAN_RETURN;
}

void FAN_cmdSddInit(FAN_Hash *ret, char **params)
{	
	FAN_ENTER;
	FAN_cmdSddInit();
	FAN_RETURN;
}

void FAN_cmdSddInit()
{	
	FAN_ENTER;
	FAN_RETURN;
}

void FAN_cmdSysVersion(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        FAN_xlog(FAN_DEBUG,"version");
	char *ver;
	char *build = FAN_BuildNumber::aGetBuild();
	asprintf(&ver, "%s  / FAN build: %s", FAN::app->config->getValue("version"), build);
	free(build);
        ret->insert( "return", "TRUE" );
        ret->insert( "returnmsg", ver);
	free(ver);
	FAN_RETURN;
}

void FAN_cmdSysEndian(FAN_Hash *ret, char **params)
{
	FAN_ENTER;

        char *endian = NULL;
	
	FAN_aGetParam(params, &endian, 0);

        if(endian != NULL)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

		conf->config->insert("remoteEndian", endian);

        	ret->insert( "return", "TRUE");
        	ret->insert( "returnmsg", "OK");

		free(endian);
	}else
	{
	       	ret->insert( "return", "FALSE");
        	ret->insert( "returnmsg", "wrong param");
	}
	FAN_RETURN;
}

void FAN_cmdSysSetBufferSize(FAN_Hash *ret, char **params)
{
	FAN_ENTER;

        char *size = NULL;
	
	FAN_aGetParam(params, &size, 0);

        if(size != NULL && atoi(size) > 0)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

		conf->config->insert("bufferSize", size);

        	ret->insert( "return", "TRUE");
        	ret->insert( "returnmsg", "OK");

		free(size);
	}else if(size != NULL)
	{
		free(size);
	}
	{
	       	ret->insert( "return", "FALSE");
        	ret->insert( "returnmsg", "wrong param");
	}
	FAN_RETURN;	
}

void FAN_cmdSysUptime(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        FAN_xlog(FAN_DEBUG,"uptime");

        char *buf;

        int tot=(int)time(0)-(int)FAN_start;
                asprintf(&buf,"Up since : %d day(s) %.2d:%.2d:%.2d",tot/(3600*24),(tot%(3600*24))/3600,(tot%3600)/60,tot%60);
        ret->insert( "return", "TRUE");
        ret->insert( "returnmsg", buf);

        free(buf);
	FAN_RETURN;	
}

void FAN_cmdClearText(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        if(FAN::app->threaded==1)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);

                if(conf != NULL)
                {
                        conf->clearText = 1;
                        ret->insert("return", "TRUE");
                        ret->insert("returnmsg", "ClearText ON");
                }else   
                {
                        ret->insert("return", "FALSE");
                        ret->insert( "returnmsg", "ClearText FAILED");
                }       
        }else   
        {
                FAN::app->clearText = 1;
                ret->insert("return", "ClearText ON");
        }       
	FAN_RETURN;	
}       

void FAN_cmdNoClearText(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        if(FAN::app->threaded==1)
        {
                FAN *conf = (FAN*)pthread_getspecific(FAN::app->threadFAN);
                
                if(conf != NULL)
                {
                        conf->clearText = 0;
                        ret->insert("return", "TRUE");
                        ret->insert("returnmsg", "ClearText OFF");
                }else
                {
                        ret->insert("return", "FALSE");
                        ret->insert( "returnmsg", "ClearText FAILED");
                }
        }else
        {
                FAN::app->clearText = 0;
                ret->insert("return", "TRUE");
                ret->insert( "returnmsg", "ClearText OFF");
        }
	FAN_RETURN;	
}

void FAN_cmdSysStatus(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
	FAN *app=FAN::app;
	FAN_xlog(FAN_DEBUG,"status");
	char *buf;
	char *build = FAN_aGetBuild();
	string bld(build);
	free(build);
    int tot=(int)time(0)-(int)FAN_start;
    asprintf(&buf,"%d day(s) %.2d:%.2d:%.2d",tot/(3600*24),(tot%(3600*24))/3600,(tot%3600)/60,tot%60);
	string message ("\n"+bld+" STATUS OVERVIEW\n");
	message=message+"------------------------------------------------------------------\n";
	message=message+"Compile time OS      : "+FAN_BuildNumber::buildos+"\n";
	message=message+"Uptime               : "+buf+"\n";
	free(buf);
//    asprintf(&buf,"%d",app->threadFAN->count());
	message=message+"Connected clients    : "+buf+"\n";
	message=message+"Registered threads   : "+buf+"\n";
//	free(buf);
    asprintf(&buf,"%d ",app->sysStatus.connectionCounter);
	message=message+"Total served         : "+buf+"\n";
	free(buf);
    asprintf(&buf,"%d ",FAN_getThreadId());
	message=message+"I am thread number   : "+buf+"\n";
	free(buf);
    ret->insert( "returnmsg", (char*)message.c_str());
    ret->insert( "return", "TRUE");
	FAN_RETURN;	
}

void FAN_cmdExit(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        FAN_xlog(FAN_DEBUG,"exit");
        ret->insert("return", "TRUE");

        ret->insert("returnmsg", "NOT IMPLEMENTED");
	FAN_RETURN;	
}

void FAN_cmdSddReadConf(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        int size = FAN::app->config->getLength();
        void **list = (void **)malloc(size*sizeof(void*));
        int i;

        FAN::app->config->getKeys((char**)list);
        for(i=0; i<size; i++)
        {
                ret->insert((char*)*(list+i), FAN::app->config->getValue((char*)*(list+i)));
        }
        ret->insert("return", "TRUE");
        free(list);
	FAN_RETURN;	
}

void FAN_cmdHelp(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        char *tmp, *p;

        int i;

        asprintf(&tmp, "%s", APPEND);

        FAN_xlog(FAN_DEBUG|FAN_SOCKET, "help");

	char *build = FAN_aGetBuild();
	char *build2;
	asprintf(&build2, "%s\n", build);
	free(build);

        p = tmp;
	
        asprintf(&tmp, p, build2, APPEND);
	free(build2);
        free(p);

        int size = FAN_cmd->getPointerLength();
        char **list = (char**)malloc(size*sizeof(char*));


	// unused? --OM
	//FAN *conf = (FAN*)FAN::app->threadFAN->search((unsigned long int)pthread_self());

        FAN_cmd->getPointerKeys(list);
        for(i=0; i<size; i++)
        {
		char *key = *(list+i);
		FAN_ProtocolCommand *c = (FAN_ProtocolCommand*)FAN_cmd->getPointer(key);

		if(c != NULL && c->available)
		{
                	p = tmp;
                	asprintf(&tmp, p, key, APPEND);
                	free(p);
                	p = tmp;
                	asprintf(&tmp, p, " ", APPEND);
                	free(p);
		}
        }
        free(list);
	
        p = tmp;
        asprintf(&tmp, p, "\n", END);
        free(p);

        ret->insert("return", "TRUE");
        ret->insert("returnmsg", tmp);

        free(tmp);
	FAN_RETURN;	
}

void FAN_cmdConnect(FAN_Hash *ret, char **params)
{
	FAN_ENTER;
        char *user, *pass;
	// unused? --OM
        //int stat;
        
        FAN_xlog(FAN_DEBUG|FAN_SOCKET,"connect");
        
        FAN_aGetParam(params, &user, 0);
        FAN_xlog(FAN_DEBUG|FAN_SOCKET,user);     
        FAN_aGetParam(params, &pass, 1);
        FAN_xlog(FAN_DEBUG|FAN_SOCKET,pass);     

	if(user == NULL || pass == NULL)
	{
		FAN_xlog(FAN_ERROR, "user and pwd required");
		ret -> insert("RETURN", "FALSE");
		ret -> insert("RETURNMSG", "user and pwd required");
		
		if(user != NULL) free(user);
		if(pass != NULL) free(pass);
	}else // IRIX QUICK HACK
	{
        	ret->insert("RETURN", "TRUE");
        	ret->insert("RETURNMSG", "connected");
/*		unsigned int wc = FAN_generateWebCode();
		char *buf;
		asprintf(&buf,"%u",wc);
       	 	ret->insert("session_id",buf);
		free(buf);*/
		free(user);
		free(pass);
	}
	FAN_RETURN;
}

