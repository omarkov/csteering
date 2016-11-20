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



#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "FANClasses.h"
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>

/**
 * Threading structure 
 *
 * WARNING: Do not modify any members
 */
typedef struct {
    char *txt;
    char *buf;			// unused
    int sock;			// socket handle
    int id;			// unused
    int uid;			// uid to drop permissions to
    void *ptr;			// generic pointer
} FAN_Child;

void (*FAN_cleanupFunction) (int);

void FAN_defaultCleanup(int a)
{
    FAN_ENTER;
    FAN_RETURN;
}

int FAN_wait(long s, long ns)
{
    FAN_ENTER;
    struct timespec ts = { 0, 0 };

    // --OM: there's no clock_gettime on Darwin
#if __IRIX__ || __LINUX__ || __DARWIN_OSX__
    pthread_mutex_t m;
    pthread_cond_t c;
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&c, NULL);
    pthread_mutex_lock(&m);
#ifndef __DARWIN_OSX__
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv = { 0, 0 };
    gettimeofday(&tv, 0);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#endif
    ts.tv_sec += s;
    ts.tv_nsec += ns;
    pthread_cond_timedwait(&c, &m, &ts);
    pthread_mutex_unlock(&m);
    pthread_cond_destroy(&c);
    pthread_mutex_destroy(&m);
    FAN_RETURN 0;
#else
    ts.tv_sec = s;
    ts.tv_nsec = ns;
    FAN_RETURN pthread_delay_np(&ts);
#endif
}


FAN_ThreadedDaemon::FAN_ThreadedDaemon(char *cfgfile, char *chost, char *cport)
{
    this->confFile = cfgfile;
    this->bShutdown = false;
    this->confHost = chost;
    this->confPort = cport;
    this->isFileSocket = false;
    this->acceptSocket = 0;
    FAN_cleanupFunction = &FAN_defaultCleanup;
}

FAN_ThreadedDaemon::FAN_ThreadedDaemon(char *cfgfile, char *unixSocketFile)
{
    this->confFile = cfgfile;
    this->unixSocket = unixSocketFile;
    this->bShutdown = false;
    this->isFileSocket = true;
    this->acceptSocket = 0;
    FAN_cleanupFunction = &FAN_defaultCleanup;
}

void FAN_registerCleanup(void (*fkt) (int))
{
    FAN_ENTER;
    FAN_cleanupFunction = fkt;
    FAN_RETURN;
}

void *FAN_dispatch(void *x)
{
    FAN_ENTER;

    char *buf, *pcmd, **params;
    int size, s;
    void **list;
    FAN_Hash *hash;
    FAN_Child *child = (FAN_Child *) x;
    FAN_Hash *cmd;
    char *value;
    pthread_t thread = pthread_self();
    FAN *conf = new FAN(FAN::app);

    s = child->sock;
    cmd = (FAN_Hash *) child->ptr;

    pthread_setspecific(FAN::app->threadFAN, (void *) conf);

    FAN_swrite(s, "#FANSH/> ");

    char *cBufferSize = conf->config->getValue("bufferSize");

    int bufferSize = 0;
    if (cBufferSize != NULL) {
	bufferSize = atoi(cBufferSize);
    }

    if (bufferSize <= 0)
	bufferSize = 2048;

    int oldBufferSize = 0;
    // FAN_xlog(FAN_ERROR, "before eeadline");
    char *buffer = new char[bufferSize];
    unsigned char *binaryData = NULL;
    int binaryDataSize = 0;
    int binaryParamsMaxCount = 0;
    void **binaryParamsArray = NULL;
    int *binaryParamPositions = NULL;


    while (FAN::app != NULL && FAN::app->theDaemon->running() && (buf = FAN_areadline(s, false, bufferSize, buffer)) != NULL && FAN::app != NULL) {
	FAN_parseCmd(buf, &pcmd, &params);

	if (strcasecmp(pcmd, "BINARYPUSH") == 0) {
	    char *fkt = NULL;
	    char *templ = NULL;
	    char *txt = NULL;
	    int asize = 0;
	    FAN_aGetParam(params, &fkt, 0);

	    FAN_swrite(s, "RETURN");
	    FAN_swrite(s, "=\"");
	    if (conf->clearText) {
		FAN_swrite(s, "TRUE");
	    } else {
		FAN_Base64::aencode64("TRUE", &value);
		FAN_swrite(s, value);
		free(value);
	    }
	    FAN_swrite(s, "\"\n");
	    FAN_swrite(s, "EOF\n");

	    char *command = NULL;

	    if (conf->use != NULL) {
		asprintf(&command, "%s::%s", conf->use, fkt);
	    } else {
		command = strdup(fkt);
	    }

	    FAN_ProtocolCommand *theCmd = (FAN_ProtocolCommand *) cmd->getPointer(command);

	    ZAP_ARRAY(params);
	    free(buf);

	    if (theCmd != NULL && theCmd->available) {
		while ((templ = FAN_arecvlineC(s, false, bufferSize, buffer)) != NULL && strlen(templ) > 1) {
		    // FAN_swrite(s,"OK\n");
		    int type;

		    int count = FAN_getParamCount(templ);
		    char *types = templ;

		    if (binaryParamsArray == NULL) {
			binaryParamsArray = new void *[count + 2];
			binaryParamPositions = new int[count + 1];
			binaryParamsMaxCount = count;
		    } else if (count > binaryParamsMaxCount) {
			ZAP_ARRAY(binaryParamsArray);
			ZAP_ARRAY(binaryParamPositions);
			binaryParamsArray = new void *[count + 2];
			binaryParamPositions = new int[count + 1];
			binaryParamsMaxCount = count;
		    }

		    binaryParamsArray[0] = (void *) count;

		    int tsize = 0;

		    int i = 0;
		    for (; i < count; i++) {
			type = FAN_getNextType(&types, &txt, &asize);
			int size = FAN_getParamSize(txt, type == FAN_STRUCT) * asize;
			binaryParamPositions[i] = tsize;
			tsize += size;
		    }
		    binaryParamPositions[i] = tsize;

		    if (binaryData == NULL) {
			binaryData = new unsigned char[tsize];
			binaryDataSize = tsize;
		    } else if (tsize > binaryDataSize) {
			ZAP_ARRAY(binaryData);
			binaryData = new unsigned char[tsize];
			binaryDataSize = tsize;
		    }

		    if (FAN_binaryrecv(s, templ, tsize, (unsigned char *) binaryData)) {
			for (i = 0; i < count; i++) {
			    if (binaryParamPositions[i + 1] - binaryParamPositions[i] > 0 && binaryData != NULL)
				binaryParamsArray[i + 1] = binaryData + binaryParamPositions[i];
			    else
				binaryParamsArray[i + 1] = NULL;
			}

			FAN_xlog(FAN_DEBUG | FAN_SOCKET, "processing command : %s as thread 0x%x", command, FAN_getThreadId());
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "Before %s()", fkt);
			theCmd->fkt(NULL, (char **) binaryParamsArray);
			FAN_xlog(FAN_DEBUG | FAN_INTERNAL, "After %s()", fkt);
		    }
			
		    free(templ);
		}
		FAN_swrite(s, "\n");
	    }
	    free(command);
	    free(fkt);

	    continue;
	}

	if (strcasecmp(pcmd, "EXIT") == 0 || strcasecmp(pcmd, "QUIT") == 0 || strcasecmp(pcmd, ".") == 0) {
	    FAN_swrite(s, "RETURN");
	    FAN_swrite(s, "=\"");
	    if (conf->clearText) {
		FAN_swrite(s, "TRUE");
	    } else {
		FAN_Base64::aencode64("TRUE", &value);
		FAN_swrite(s, value);
		free(value);
	    }
	    FAN_swrite(s, "\"\n");
	    FAN_swrite(s, "EOF\n");

	    (*FAN_cleanupFunction) ((int) thread);
	    FAN *conf = (FAN *) pthread_getspecific(FAN::app->threadFAN);
	    pthread_setspecific(FAN::app->threadFAN, NULL);

	    ZAP_ARRAY(binaryData);
	    ZAP_ARRAY(binaryParamsArray);
	    ZAP_ARRAY(binaryParamPositions);

	    ZAP(conf);
	    free(buf);
	    ZAP(child);
	    ZAP_ARRAY(buffer);

	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Closing client socket %d", s);
	    shutdown(s, SHUT_RDWR);
	    close(s);
	    FAN_TRACEBACK;
	    pthread_exit(NULL);
	}

	char *command = NULL;

	if (conf->use != NULL) {
	    asprintf(&command, "%s::%s", conf->use, pcmd);
	} else {
	    command = strdup(pcmd);
	}

	FAN_ProtocolCommand *theCmd = (FAN_ProtocolCommand *) cmd->getPointer(command);

	if (theCmd != NULL && theCmd->available) {
	    hash = new FAN_Hash();

	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "processing command : %s as thread 0x%x", command, FAN_getThreadId());

	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Before %s()", pcmd);
	    theCmd->fkt(hash, params);
	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "After %s()", pcmd);

	    size = hash->getLength();
	    if (size == 0) {
		hash->insert("RETURN", "false");
		hash->insert("RETURNMSG", "no msg");
		size = 2;
	    }
	    list = (void **) malloc(size * sizeof(void *));
	    hash->getKeys((char **) list);
	    FAN_swrite(s, "\n");
	    for (int i = 0; i < size; i++) {
		FAN_swrite(s, (char *) *(list + i));
		FAN_swrite(s, "=\"");
		if (conf->clearText) {
		    FAN_swrite(s, hash->getValue((char *) *(list + i)));
		} else {
		    FAN_Base64::aencode64(hash->getValue((char *) *(list + i)), &value);
		    if (value != NULL) {
			FAN_swrite(s, value);
			free(value);
		    }
		}
		FAN_swrite(s, "\"\n");
	    }
	    free(list);
	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "After Hash buildup");
	    FAN_swrite(s, "\nEOF\n");
	    if (conf->clearText) {
		char *prompt = NULL;
		if (conf->use != NULL)
		    asprintf(&prompt, "#FANSH:/%s> ", conf->use);
		else
		    asprintf(&prompt, "#FANSH:/> ");

		FAN_swrite(s, prompt);

		free(prompt);
	    }
	    ZAP(hash);
	} else {
	    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "command not understood : %s", command);
	    FAN_swrite(s, "\nRETURNMSG=\"Command not understood\"\nRETURN=FALSE\nEOF\n");
	    if (conf->clearText) {
		char *prompt = NULL;
		if (conf->use != NULL)
		    asprintf(&prompt, "#FANSH:/%s> ", conf->use);
		else
		    asprintf(&prompt, "#FANSH:/> ");

		FAN_swrite(s, prompt);
		free(prompt);
	    }
	}
	if (params != NULL)
	    ZAP_ARRAY(params);

	free(command);
	free(buf);

	oldBufferSize = bufferSize;
	cBufferSize = conf->config->getValue("bufferSize");

	if (cBufferSize != NULL) {
	    bufferSize = atoi(cBufferSize);
	}

	if (bufferSize <= 0)
	    bufferSize = 2048;

	if (bufferSize != oldBufferSize) {
	    ZAP_ARRAY(buffer);
	    buffer = new char[bufferSize];
	}
	// FAN_xlog(FAN_ERROR, "before readline");
    }

    (*FAN_cleanupFunction) ((int) thread);

    ZAP_ARRAY(binaryData);
    ZAP_ARRAY(binaryParamsArray);
    ZAP_ARRAY(binaryParamPositions);

    ZAP(conf);
    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Closing client socket %d", s);
    shutdown(s, SHUT_RDWR);
    close(s);

    ZAP(child);
    ZAP_ARRAY(buffer);

    FAN_TRACEBACK;
    pthread_exit(NULL);

    // shut up the MIPSPro compiler --OM
    return NULL;
}

int FAN_getBindSocket(char *listenip, int port)
{
    FAN_ENTER;
    int sock;

    struct sockaddr_in sin;
    if ((!strlen(listenip))) {
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Try to bind on all interfaces");
	inet_aton("0.0.0.0", &(sin.sin_addr));
    } else {
	inet_aton(listenip, &(sin.sin_addr));
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Try to bind to ip %s:%d", listenip, port);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons((u_short) port);

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
	FAN_xlog(FAN_ERROR, "Cant create tcp socket");
	FAN_RETURN false;
    }
    // This code should avoid the "hanging" socket after server kill
    int on = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int))) {
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "REUSEADDR FAILED");
    }
    int on2 = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on2, sizeof(int))) {
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "KEEPALIVE FAILED");
    }

    /*
     * struct linger l; l.l_onoff=1; l.l_linger=1; if(setsockopt(sock,SOL_SOCKET,SO_LINGER, (char *)&l,sizeof(l))) { FAN_xlog(FAN_DEBUG | FAN_SOCKET, "LINGER FAILED"); } int p=1; if(fcntl(sock,F_SETFD,p)==-1) { FAN_xlog(FAN_DEBUG | FAN_SOCKET,"FCNTL FAILED"); } 
     */

    FAN_xlog(FAN_DEBUG, "Binding socket on port %d", port);
    if (bind(sock, (struct sockaddr *) &sin, sizeof sin) < 0) {
	FAN_xlog(FAN_ERROR, "Cant assign address");
	FAN_RETURN false;
    }
    if (listen(sock, 5) < 0) {
	FAN_xlog(FAN_ERROR, "Cant turn to listening mode");
	FAN_RETURN false;
    }
    FAN_RETURN sock;
}

int FAN_getBindFileSocket(char *listensock)
{
    FAN_ENTER;
    int sock;
    struct sockaddr_in fsin;
    FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Listening on file %s", listensock);

    fsin.sin_family = AF_LOCAL;

    if ((sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
	FAN_xlog(FAN_ERROR, "Cant create file socket.");
	return false;
    }
    if (bind(sock, (struct sockaddr *) &fsin, sizeof fsin) < 0) {
	FAN_xlog(FAN_ERROR, "Cant bind on file socket");
	return false;
    }
    if (listen(sock, 5) < 0) {
	FAN_xlog(FAN_ERROR, "Cant turn to listening mode");
	return false;
    }
    FAN_RETURN sock;
}

bool FAN_ThreadedDaemon::running()
{
    return !bShutdown;
}

void FAN_ThreadedDaemon::shutdownDaemon()
{
    if (acceptSocket > 0) {
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Closing server socket %d", acceptSocket);
	shutdown(acceptSocket, SHUT_RDWR);
	close(acceptSocket);
    }

    bShutdown = true;

    acceptSocket = 0;
}

bool FAN_ThreadedDaemon::binded()
{
    FAN_ENTER;
    FAN_RETURN(acceptSocket != 0);
}

bool FAN_ThreadedDaemon::bindDaemon()
{
    FAN_ENTER;
    FAN_RETURN bindDaemon(NULL);
}

bool FAN_ThreadedDaemon::bindDaemon(void *(*master) (void *))
{
    FAN_ENTER;
    FAN_RETURN bindDaemon(master, NULL, 0);
}

void *FAN__handler(void *vreg)
{
    FAN_ENTER;
    FAN_Hash *reg = (FAN_Hash *) vreg;
    FAN_Com *com = (FAN_Com *) reg->getPointer("COM");

    void *(*handler) (FAN_Hash *, void *);
    void *(*idle) (FAN_Hash *, void *);
    void *(*init) (FAN_Hash *, void *);


    init = (void *(*)(FAN_Hash *, void *)) reg->getPointer("INITAL");
    if (init != NULL)
	init(reg, NULL);

    handler = NULL;
    idle = (void *(*)(FAN_Hash *, void *)) reg->getPointer("IDLE");

    // while(FAN::app->theDaemon == NULL || FAN::app->theDaemon->binded())
    while (FAN::app != NULL) {
	// FAN_err("MQueue: %d", com->MsgQueue->getSize());

	if (idle != NULL)
	    idle(reg, NULL);

	FAN_Msg *msg = FAN_peekMessage(com);
	if (msg != NULL) {
	    handler = (void *(*)(FAN_Hash *, void *)) reg->getPointer(msg->type);

	    if (handler != NULL) {
		void *ret = handler(reg, msg->value);
		if (msg->reply != NULL) {
		    FAN_postMessage(msg->reply, msg->replyType, com, ret);
		}
	    }
	    FAN_recycleMsg(com, msg);
	} else {
	    FAN_err("ERRRRROR !!!!!");
	    exit(0);
	}
    }
    FAN_RETURN NULL;
}

FAN_Hash *FAN_initMasterHandler()
{
    FAN_ENTER;
    FAN_Hash *reg = new FAN_Hash();
    reg->insertPointer("COM", FAN_initMessages());
    FAN_registerMaster(&FAN__handler, (void *) reg);
    FAN_RETURN reg;
}

FAN_Hash *FAN_initWorkerHandler()
{
    FAN_ENTER;
    FAN_Hash *reg = new FAN_Hash();
    FAN_RETURN reg;
}

void FAN_startWorkers(FAN_Hash * reg, int count)
{
    FAN_ENTER;
    FAN_registerWorkers(&FAN__handler, count, reg);
    delete reg;
    FAN_RETURN;
}

void FAN_registerHandler(FAN_Hash * reg, char *fkt, void *(*handler) (FAN_Hash *, void *))
{
    FAN_ENTER;
    reg->insertPointer(fkt, (void *) handler);
    FAN_RETURN;
}

bool FAN_registerMaster(void *(*master) (void *), void *p)
{
    FAN_ENTER;
    pthread_t t;

    pthread_attr_t p_attr;
    pthread_attr_init(&p_attr);
    pthread_attr_setdetachstate(&p_attr, PTHREAD_CREATE_DETACHED);

    if (int v = pthread_create(&t, &p_attr, master, p)) {
	FAN_xlog(FAN_ERROR, "pthread_create() failed with %d (%s)", v, strerror(errno));
	FAN_RETURN false;
    }
    FAN_RETURN true;
}

bool FAN_registerMaster(void *(*master) (void *))
{
    FAN_ENTER;
    FAN_RETURN FAN_registerMaster(master, NULL);
}

bool FAN_registerWorkers(void *(*worker) (void *), int count, FAN_Hash * p)
{
    FAN_ENTER;
    pthread_t t;

    pthread_attr_t p_attr;
    pthread_attr_init(&p_attr);
    pthread_attr_setdetachstate(&p_attr, PTHREAD_CREATE_DETACHED);

    void *p2;

    for (int i = 0; i < count; i++) {
	if (p == NULL)
	    p2 = (long *) i;
	else {
	    FAN_Hash *hash = new FAN_Hash();
	    p->copyPointers(hash);
	    (hash)->insertPointer("COM", FAN_initMessages());
	    char *id = NULL;
	    asprintf(&id, "%d", i);
	    (hash)->insert("ID", id);
	    free(id);
	    p2 = (FAN_Hash *) hash;
	}
	if (int v = pthread_create(&t, &p_attr, worker, p2)) {
	    FAN_xlog(FAN_ERROR, "pthread_create() failed with %d (%s)", v, strerror(errno));
	    FAN_RETURN false;
	}
    }
    FAN_RETURN true;
}

bool FAN_registerWorkers(void *(*worker) (void *), int count)
{
    FAN_ENTER;
    FAN_RETURN FAN_registerWorkers(worker, count, NULL);
}


bool FAN_ThreadedDaemon::bindDaemon(void *(*master) (void *), void *(*worker) (void *), int count)
{
    FAN_ENTER;
    struct sockaddr_in fsin;
    int ssock = 0, alen;
    char *remoteHost;

    pthread_attr_t p_attr;
    pthread_attr_init(&p_attr);
    pthread_attr_setdetachstate(&p_attr, PTHREAD_CREATE_DETACHED);


    FAN_Child *a;

    interupt = 1;

    FAN::app->threaded = 1;
    acceptSocket = -1;
    FAN::app->theDaemon = this;

    while (1) {
	if (!running()) {
	    for (int w = 0; w < 5; w++) {
		    FAN_err("Waiting for threads ...");
		    FAN_wait(1, 0);
	    }

            if (FAN::app != NULL)
	        FAN::app->theDaemon = NULL;
	    FAN_RETURN true;
	}
	if (interupt) {
	    interupt = 0;

	    if (acceptSocket > 0) {
		shutdown(acceptSocket, SHUT_RDWR);
		close(acceptSocket);
	    }

	    if (!isFileSocket) {
		char *listenip = FAN::app->config->getValue(confHost);
		char *listenport = FAN::app->config->getValue(confPort);

		if (listenip == NULL || listenport == NULL)
		    FAN_plog(FAN_ERROR, "address/port missing.");

		acceptSocket = FAN_getBindSocket(listenip, atoi(listenport));
		if (!acceptSocket) {
		    acceptSocket = -1;
		    FAN_RETURN false;
		}
	    } else {
		char *listensock = NULL;
		FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Creating file socket on file %s", listensock);
		listensock = (char *) FAN::app->config->getPointer(unixSocket);
		acceptSocket = FAN_getBindFileSocket(listensock);
		if (!acceptSocket) {
		    acceptSocket = -1;
		    FAN_RETURN false;
		}
	    }

	    alen = sizeof(fsin);

	    // socket sichern um sie im signalhandler schliessen zu können
	    // FAN_::app->acceptSocket=acceptSocket;

	    if (master != NULL) {
		FAN_registerMaster(master);
	    }

	    if (worker != NULL) {
		FAN_registerWorkers(worker, count);
	    }
	}

	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Waiting for connection");
#ifdef __IRIX__
	if ((ssock = accept(acceptSocket, (struct sockaddr *) &fsin, &alen)) < 0)
#else
	if ((ssock = accept(acceptSocket, (struct sockaddr *) &fsin, (socklen_t *) & alen)) < 0)
#endif
	{
	    if (errno == EINTR)
		continue;
	    if (errno == ECONNABORTED)
		continue;
	    continue;		// maybe wrong ...
	}
	if (acceptSocket == 0) {
	    if (ssock) {
		shutdown(ssock, SHUT_RDWR);
		close(ssock);
	    }

	    continue;
	}

	if (!ssock) {

	    continue;

	    // FAN_xlog(FAN_ERROR|FAN_SOCKET,"accept() failed : %s",strerror(errno));
	}

	a = new FAN_Child;
	a->ptr = (FAN_Hash *) FAN_cmd;
	a->sock = ssock;

	remoteHost = (char *) inet_ntoa(fsin.sin_addr);
	FAN_xlog(FAN_DEBUG | FAN_SOCKET, "dispatching for %s (sock%d)", remoteHost, a->sock);
	// free(remoteHost);
	pthread_t t;

	FAN::app->sysStatus.connectionCounter++;

	if (int v = pthread_create(&t, &p_attr, &FAN_dispatch, a)) {
	    FAN_xlog(FAN_ERROR, "pthread_create() failed with %d (%s)", v, strerror(errno));
	}
    }

    if (FAN::app != NULL)
	FAN::app->theDaemon = NULL;

    FAN_RETURN false;
}

void FAN_parseCmd(char *line, char **cmd_pointer, char ***pparams)
{
    FAN_ENTER;
    char *param_pointer;
    int length = strlen(line);
    int i;
    int status = FAN_BEFORE_CMD;
    char **params = new char *[20];

    *pparams = params;

    *cmd_pointer = line;
    param_pointer = line;

    char **len;
    len = params;
    params++;
    int pcount = 0;


    for (i = 0; i < length && status != FAN_AFTER_PARAM; i++) {
	switch (status) {
	case FAN_BEFORE_CMD:
	    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r') {
		(char *) (*cmd_pointer)++;
	    } else {
		if (line[i] >= 'a' && line[i] <= 'z') {
		    line[i] = line[i] + 'A' - 'a';
		}
		status = FAN_CMD;
	    }
	    break;

	case FAN_CMD:

	    if (line[i] == ' ' || line[i] == '\t') {
		line[i] = '\0';
		status = FAN_AFTER_CMD;

	    } else if (line[i] == '(') {
		line[i] = '\0';
		status = FAN_BEFORE_PARAM;
		param_pointer += i + 1;
	    } else if (line[i] == '\n' || line[i] == '\r') {
		line[i] = '\0';
		status = FAN_AFTER_PARAM;

	    } else {
		if (line[i] >= 'a' && line[i] <= 'z') {
		    line[i] = line[i] + 'A' - 'a';
		}
	    }
	    break;

	case FAN_AFTER_CMD:
	    if (line[i] == '(') {
		status = FAN_BEFORE_PARAM;
		param_pointer += i + 1;
	    } else if (line[i] == '\n' || line[i] == '\r') {
		status = FAN_AFTER_PARAM;
	    }
	    break;

	case FAN_BEFORE_PARAM:

	    if (line[i] == ' ' || line[i] == '\t' || line[i] == ',') {
		param_pointer++;
	    } else if (line[i] == '"') {
		param_pointer++;
		status = FAN_EXT_PARAM;
		*params = param_pointer;
		params++;
		pcount++;
	    } else if (line[i] == ')') {
		line[i] = '\0';
		status = FAN_AFTER_PARAM;
	    } else {
		status = FAN_PARAM;
		*params = param_pointer;
		params++;
		pcount++;
	    }
	    break;

	case FAN_PARAM:
	    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n' || line[i] == '#' || line[i] == ',') {
		line[i] = '\0';
		param_pointer = line + i + 1;
		status = FAN_BEFORE_PARAM;
	    } else if (line[i] == ')') {
		line[i] = '\0';
		status = FAN_AFTER_PARAM;
	    }
	    break;

	case FAN_EXT_PARAM:
	    if (line[i] == '\\') {
		status = FAN_QUOTE;
	    } else if (line[i] == '"') {
		line[i] = '\0';
		status = FAN_BEFORE_PARAM;
		param_pointer = line + i + 1;
	    }
	    break;
	}
    }
    *len = (char *) pcount;
    FAN_RETURN;
}

FAN_ThreadedDaemon::~FAN_ThreadedDaemon()
{
}
