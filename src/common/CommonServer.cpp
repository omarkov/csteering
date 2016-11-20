#include "CommonServer.h"

// GLOBAL VARS
FAN_Com   *masterCom     = NULL;
FAN_Connection *modelConn  = NULL;
FAN_Connection *simConn  = NULL;
FAN_Connection *visConn  = NULL;
FAN_Connection *visConn2 = NULL;

char *modelBuffer = NULL;
char *simBuffer = NULL;
char *visBuffer = NULL;
char *modelHost = NULL;
char *simHost = NULL;
char *visHost = NULL;

int modelPort    = 0;
int simPort    = 0;
int visPort    = 0;

#ifdef HASWORKERS
FAN_Queue *jobQueue         = new FAN_Queue();
FAN_Queue *jobRecyclerQueue = new FAN_Queue(); 
FAN_Queue *workerQueue      = new FAN_Queue();
#endif

// WORKERS
#ifdef HASWORKERS
void *wIdle(FAN_Hash *reg, void *param)
{
    FAN_ENTER;
    FAN_Com *myCom = (FAN_Com*)reg->getPointer("COM");
    FAN_postMessage(masterCom, "getjob", myCom, myCom);
    FAN_RETURN NULL;
}
#endif
FAN_Connection *getVisConn2()
{
    FAN_ENTER;
    bool connected = false;
    if(visConn2 != NULL) connected = true;

    if(visConn2 != NULL && visConn2->isConnected())
    {
        FAN_RETURN visConn2;
    }
    if(visConn2 != NULL)
    {
	    if(visConn2 == FAN::app->sddConn)
		    FAN::app->sddConn = NULL;
	    delete visConn2;
	    visConn2 = NULL;
    }

    if(FAN::app != NULL && FAN::app->sddConn != NULL && FAN::app->sddConn->isConnected())
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Use SDD-connection as VIS-connection");
	visConn2 = FAN::app->sddConn;
        visConn2->rpc("use");
        visConn2->rpc("sys::setBufferSize", 1, visBuffer); 
    }else if(FAN::app != NULL)
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Connect to VIS-server (%s/%d)", visHost, visPort);
	visConn2 = new FAN_Connection(NULL, visHost, visPort);
	if(visConn2->connect())
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "done");
            visConn2->rpc("sys::setBufferSize", 1, visBuffer);
	}else
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "failed");
	    FAN_wait(1,0);
	    delete visConn2;
	    visConn2 = NULL;
	}
    }
    if(connected && visConn2 == NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "VIS disconnected");
    }else if(!connected && visConn2 != NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "VIS connected");
    }

    FAN_RETURN visConn2;
}


FAN_Connection *getVisConn()
{
    FAN_ENTER;
    bool connected = false;
    if(visConn != NULL) connected = true;

    if(visConn != NULL && visConn->isConnected())
    {
        FAN_RETURN visConn;
    }
    if(visConn != NULL)
    {
	    if(visConn == FAN::app->sddConn)
		    FAN::app->sddConn = NULL;
	    delete visConn;
	    visConn = NULL;
    }

    if(FAN::app != NULL && FAN::app->sddConn != NULL && FAN::app->sddConn->isConnected())
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Use SDD-connection as VIS-connection");
	visConn = FAN::app->sddConn;
        visConn->rpc("use");
        visConn->rpc("sys::setBufferSize", 1, visBuffer); 
    }else if(FAN::app != NULL)
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Connect to VIS-server (%s/%d)", visHost, visPort);
	visConn = new FAN_Connection(NULL, visHost, visPort);
	if(visConn->connect())
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "done");
            visConn->rpc("sys::setBufferSize", 1, visBuffer);
	}else
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "failed");
	    FAN_wait(1,0);
	    delete visConn;
	    visConn = NULL;
	}
    }
    if(connected && visConn == NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "VIS disconnected");
    }else if(!connected && visConn != NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "VIS connected");
    }

    FAN_RETURN visConn;
}

FAN_Connection *getSimConn()
{
    FAN_ENTER;
    bool connected = false;
    if(simConn != NULL) connected = true;

    if(simConn != NULL && simConn->isConnected())
    {
        FAN_RETURN simConn;
    }
    if(simConn != NULL)
    {
	    delete simConn;
	    simConn = NULL;
    }

    if(FAN::app != NULL)
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Connect to SIM-server (%s/%d)", simHost, simPort);
	simConn = new FAN_Connection(NULL, simHost, simPort);
	if(simConn->connect())
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "done");
            simConn->rpc("sys::setBufferSize", 1, simBuffer);
	}else
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "failed");
            FAN_wait(1,0);
	    delete simConn;
	    simConn = NULL;
	}
    }
    if(connected && simConn == NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "SIM disconnected");
    }else if(!connected && simConn != NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "SIM connected");
    }

    FAN_RETURN simConn;
}



FAN_Connection *getModelConn()
{
    FAN_ENTER;
    bool connected = false;
    if(modelConn != NULL) connected = true;

    if(modelConn != NULL && modelConn->isConnected())
    {
        FAN_RETURN modelConn;
    }
    if(modelConn != NULL)
    {
	    delete modelConn;
	    modelConn = NULL;
    }

    if(FAN::app != NULL)
    {
	FAN_xlog(FAN_INFO | FAN_MISC, "Connect to ModelServer (%s/%d)", modelHost, modelPort);
	modelConn = new FAN_Connection(NULL, modelHost, modelPort);
	if(modelConn->connect())
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "done");
            modelConn->rpc("sys::setBufferSize", 1, modelBuffer);
	}else
	{
            FAN_xlog(FAN_INFO | FAN_MISC, "failed");
            FAN_wait(1,0);
	    delete modelConn;
	    modelConn = NULL;
	}
    }
    if(connected && modelConn == NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "ModelServer disconnected");
    }else if(!connected && modelConn != NULL)
    {
        FAN_xlog(FAN_MISC | FAN_INFO, "ModelServer connected");
    }

    FAN_RETURN modelConn;
}

//MASTER
#ifdef HASWORKERS
t_job *getJobStruct(char *type, void *data, FAN_Com *reply, char *replyType)
{
    FAN_ENTER; 

    t_job *job = (t_job*)jobRecyclerQueue->pop();
    if(job == NULL)
    {
	    job = new t_job;
	    job->type = type;
	    job->reply = reply;
	    job->replyType = replyType;
    }
    job->data=data;

    FAN_RETURN job;
}

void recycleJob(t_job *job)
{
	jobRecyclerQueue->push((void*)job);
//	free(tjob);
}

void *mPutJob(FAN_Hash *reg, void *job)
{
    FAN_ENTER;
    FAN_Com *worker = (FAN_Com*)workerQueue->pop();

    if(worker != NULL)
    {
        t_job *tjob = (t_job*)job;
        FAN_postMessage(worker, tjob->type, tjob->reply, tjob->replyType, tjob->data);
	recycleJob(tjob);
    }
    else
    {
        jobQueue->push(job);
    }
    FAN_RETURN NULL;
}

void *mGetJob(FAN_Hash *reg, void *worker)
{
    FAN_ENTER;
    t_job  *job    = (t_job*)jobQueue->pop();

    if(job == NULL)     
        workerQueue->push(worker);
    else
    {
        FAN_postMessage((FAN_Com*)worker, job->type, job->reply, job->replyType, job->data);
	recycleJob(job);
    }

    FAN_RETURN NULL;
}
#endif
