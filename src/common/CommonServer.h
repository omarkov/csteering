#ifndef _CS_H
#define _CS_H

#include "FANClasses.h"

// DEFINES
#define OK	0x1
#define FAILED  0x2

#ifdef HASWORKERS
typedef struct t_job
{
   void *data;
   char *type;
   FAN_Com *reply;
   char *replyType;
};
#endif

// GLOBAL VARS
extern FAN_Com   *masterCom;
extern FAN_Connection *modelConn;
extern FAN_Connection *simConn;
extern FAN_Connection *visConn;
extern FAN_Connection *visConn2;

extern char *modelBuffer;
extern char *simBuffer;
extern char *visBuffer;
extern char *modelHost;
extern char *simHost;
extern char *visHost;

extern int modelPort;
extern int simPort;
extern int visPort;

#ifdef HASWORKERS
extern FAN_Queue *jobQueue;
extern FAN_Queue *jobRecyclerQueue;
extern FAN_Queue *workerQueue;
#endif

FAN_Connection *getVisConn();
FAN_Connection *getVisConn2();
FAN_Connection *getSimConn();
FAN_Connection *getModelConn();

#ifdef HASWORKERS 
t_job *getJobStruct(char *type, void *data, FAN_Com *reply, char *replyType);
void recycleJob(t_job *job);

// MASTER 
void *mGetJob(FAN_Hash *reg, void *worker);
void *mPutJob(FAN_Hash *reg, void *job);

// WORKER
void *wIdle(FAN_Hash *reg, void *param);
#endif
#endif
