// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Dominik Rössler <dominik@freshx.de> 
//   All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  o Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer. 
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution. 
//  o Neither the name of the author nor the names of the contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <assert.h>

#include <stdlib.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "CommonServer.h"
#include "ModelServer.h"
//#include "RemoteInterface.h"
#include "csmodeltriangulation.h"
#include "csmodelcontroller.h"
#include "voxel/voxelization/voxelization.h"
#include "voxel/geom/node_index.h"

using namespace std;

#include "mpi.h"
#include "SimCommunicator.h"
#include "types.h"

#define EPS 0.00001
#define EQ(a,b) abs(a-b) < EPS
#define NEQ(a,b) !EQ(a,b)

FAN *g_fan = NULL;
FAN_Com *simMasterCom = NULL;
FAN_Com *simStartMasterCom = NULL;

CSModelController *csmdlCon = NULL;
SimCommunicator simulation;

// Simulation States 
bool simPaused = true;
bool simStarted = false;
bool simFilterLoop = false;
bool sim_waitForPause = false;
bool sim_waitForStart = false;

// Update Trigger
bool simUpdated   = false;
bool modelUpdated = false;
bool modelChanged = false;
double *lastRot = NULL;

// Simulation Parameters
int simUpdateRate = 10;
double simScaleX = 1.7;         // Simulationsraumfaktor X
double simScaleY = 1.7;         // Simulationsraumfaktor Y
double simScaleZ = 1.7;         // Simulationsraumfaktor Z
double stdDensity = 0.5;
double stdAcceleration = 0.05;
double stdRelaxationValue = 1.85;

// Voxelisation Parameters
int voxelRes = 6;               // Voxelaufloesung
double voxelScale = 0.3;        // Voxelraumfaktor
double g_voxelSize = 0.0;
Point g_min;
Point g_max;

FAN_Hash *samples = new FAN_Hash();
Voxelization *voxelization = NULL;
Voxels *voxels = NULL;

void *initController()
{
    csmdlCon = new CSModelController();
    return NULL;
}

void setServerStatus(FAN_Connection * conn, char *text, char *progress)
{
    if (visConn != NULL) {
        visConn->rpc("vis::setServerStatus", 2, text, progress);
    }
}

bool updateSim()
{
    if (csmdlCon != NULL) {
        setServerStatus(visConn, "Voxelizing model", "0");
        if (voxelization != NULL) {
            try {
                voxels = voxelization->getVoxels(csmdlCon->getRotatedModel(), voxelRes, csmdlCon->getNumberOfFaces(), g_min, g_max);
            }
            catch(ModelExc & e) {

                setServerStatus(visConn, "Boundary Exception", "0");
                simUpdated = false;
                voxels = NULL;
                return false;
            }
        } else {
            voxelization = csmdlCon->getVoxelization(voxelRes);
            g_min = voxelization->getCadModel()->getMinPoint();
            g_max = voxelization->getCadModel()->getMaxPoint();

            double dx = g_max.getX() - g_min.getX();
            double dy = g_max.getY() - g_min.getY();
            double dz = g_max.getZ() - g_min.getZ();

            dx = voxelScale * dx;
            dy = voxelScale * dy;
            dz = voxelScale * dz;

            g_min.setX(g_min.getX() - dx);
            g_min.setY(g_min.getY() - dy);
            g_min.setZ(g_min.getZ() - dz);
            g_max.setX(g_max.getX() + dx);
            g_max.setY(g_max.getY() + dy);
            g_max.setZ(g_max.getZ() + dz);


            voxels = voxelization->getVoxels(csmdlCon->getRotatedModel(), voxelRes, csmdlCon->getNumberOfFaces(), g_min, g_max);
            g_voxelSize = voxelization->getVoxelSize();
            cout << "VoxelSize: " << g_voxelSize << endl;
        }
        setServerStatus(visConn, "Voxelizing model", "100");
        setServerStatus(visConn, "", "");
        simUpdated = true;
    }
    return true;
}

void *mUpdateSim(FAN_Hash * reg, void *param)
{
    return (void*) updateSim();
}

void sendSimStatus(bool status)
{
    if (visConn2) {
        char *cstatus = NULL;
        asprintf(&cstatus, "%d", status);

        visConn2->rpc("vis::setSimStatus", 1, cstatus);

        MZAP(cstatus);
    }
}

void sendSimBound()
{
    int x, y, z;
    simulation.getDimensions(x, y, z);

    if (visConn2) {
        char *cx = NULL;
        char *cy = NULL;
        char *cz = NULL;

        asprintf(&cx, "%g", x * g_voxelSize);
        asprintf(&cy, "%f", y * g_voxelSize);
        asprintf(&cz, "%f", z * g_voxelSize);
        visConn2->rpc("vis::setSimulationBound", 3, cx, cy, cz);
        MZAP(cx);
        MZAP(cy);
        MZAP(cz);
    }
}

void rLogout(FAN_Hash * ret, char **params)
{
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");

    FAN_xlog(FAN_ERROR, "VIS logout. Bye");

    FAN_postMessage(masterCom, "logout", NULL, NULL);
    FAN_postMessage(simMasterCom, "logout", NULL, NULL);

}

void rLogin(FAN_Hash * ret, char **params)
{
    FAN_xlog(FAN_ERROR, "VIS login");

    if (FAN_sendMessage(masterCom, "checkLogin", NULL) == (void *) FAN_OK) {
        char *server = NULL;
        char *port = NULL;

        FAN_aGetParam(params, &server, 0);
        FAN_aGetParam(params, &port, 1);
        if (server != NULL && port != NULL) {
            MZAP(visHost);

            visHost = server;
            visPort = atoi(port);

            FAN_postMessage(simMasterCom, "clearSamples", NULL, NULL);
            FAN_postMessage(masterCom, "login", NULL, NULL);
            FAN_postMessage(simMasterCom, "login", NULL, NULL);

            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "ok");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "missing parameter(s)");
        }
        MZAP(port);
    } else {
        FAN_xlog(FAN_ERROR, "REJECTED !!!");
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "rejected");
    }
}

void *mCheckLogin(FAN_Hash * reg, void *param)
{
    if (visConn != NULL && visConn->isConnected()) {
        return (void *) FAN_ERROR;
    } else {
        return (void *) FAN_OK;
    }
}


void *mLogout(FAN_Hash * reg, void *param)
{
    if (visConn != NULL) {
        delete visConn;
        visConn = NULL;
    }

    return (void *) FAN_OK;
}

void *mSimLogout(FAN_Hash * reg, void *param)
{
    if (visConn2 != NULL) {
        delete visConn2;
        visConn2 = NULL;
    }

    return (void *) FAN_OK;
}

void *mSimLogin(FAN_Hash * reg, void *param)
{
    visConn2 = getVisConn2();

    if (simStarted) {
        sendSimBound();
    }

    if (simPaused) {
        sendSimStatus(false);
    } else {
        sendSimStatus(true);
    }

    return (void *) FAN_OK;
}

void *mLogin(FAN_Hash * reg, void *param)
{
    visConn = getVisConn();
    int size = 0;
    FACE_TRGL *myFaces = csmdlCon->getTriangulatedFaces(size);

    if (visConn != NULL && myFaces != NULL) {
        setServerStatus(visConn, "Loading model", "0");

        startData(visConn, size);
        sendFaces(visConn, myFaces, size);
        stopData(visConn);
    }

    return (void *) FAN_OK;
}

void rSetAcceleration(FAN_Hash * ret, char **params)
{
    char *accel = NULL;
    double dbAccel = 0;

    FAN_aGetParam(params, &accel, 0);

    if (accel == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [acceleration]");
    } else {
        dbAccel = atof(accel);
        if (dbAccel > 0) {
            if(NEQ(dbAccel, stdAcceleration))
            {
                stdAcceleration = dbAccel;
                simUpdated = true;
            }
            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
}

void rSetRelaxationValue(FAN_Hash * ret, char **params)
{
    char *relax = NULL;
    double dbRelax = 0;

    FAN_aGetParam(params, &relax, 0);

    if (relax == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [relaxation value]");
    } else {
        dbRelax = atof(relax);
        if (dbRelax > 0) {
            if(NEQ(stdRelaxationValue,dbRelax))
            {
                stdRelaxationValue = dbRelax;
                simUpdated = true;
            }

            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
}

void rSetSimScale(FAN_Hash * ret, char **params)
{
    char *scaleX = NULL;
    char *scaleY = NULL;
    char *scaleZ = NULL;
    double dbScaleX = 0;
    double dbScaleY = 0;
    double dbScaleZ = 0;

    FAN_aGetParam(params, &scaleX, 0);
    FAN_aGetParam(params, &scaleY, 1);
    FAN_aGetParam(params, &scaleZ, 2);

    if (scaleZ == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [voxelRes]");
    } else {
        dbScaleX = atof(scaleX);
        dbScaleY = atof(scaleY);
        dbScaleZ = atof(scaleZ);
        if (dbScaleX > 0 && dbScaleY > 0 && dbScaleZ > 0) {
            if(NEQ(simScaleX, dbScaleX) || NEQ(simScaleY, dbScaleY) || NEQ(simScaleZ, dbScaleZ))
            {
                simScaleX = dbScaleX;
                simScaleY = dbScaleY;
                simScaleZ = dbScaleZ;
                modelUpdated = true;
                simUpdated = true;
            }

            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
    MZAP(scaleX);
    MZAP(scaleY);
    MZAP(scaleZ);
}

void rSetVoxelRes(FAN_Hash * ret, char **params)
{
    char *res = NULL;
    char *scale = NULL;
    int iRes = 0;
    double dbScale = 0.0;

    FAN_aGetParam(params, &res, 0);
    FAN_aGetParam(params, &scale, 1);

    if (res == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [voxelRes] [scale=1.3]");
    } else {
        iRes = atoi(res);
        if (iRes > 0) {
            if(iRes != voxelRes)
            {
                voxelRes = iRes;
                simUpdated = true;
                modelUpdated = true;
            }

            if (scale != NULL) {
                dbScale = atof(scale);
                if (dbScale > 0)
                    voxelScale = dbScale;
            }

            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
    ZAP(res);
    ZAP(scale);
}

void rSetUpdateRate(FAN_Hash * ret, char **params)
{
    char *rate = NULL;
    int iRate = 0;

    FAN_aGetParam(params, &rate, 0);

    if (rate == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [rate]");
    } else {
        iRate = atoi(rate);
        if (iRate > 0) {
            if(simUpdateRate != iRate)
            {
                simUpdateRate = iRate;
                simUpdated = true;
            }
            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
    ZAP(rate);
}

void rSetDensity(FAN_Hash * ret, char **params)
{
    char *density = NULL;
    double dbDensity = 0;

    FAN_aGetParam(params, &density, 0);

    if (density == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [density]");
    } else {
        dbDensity = atof(density);
        if (dbDensity > 0) {
            if(NEQ(dbDensity, stdDensity))
            {
                stdDensity = dbDensity;
                simUpdated = true;
            }
            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
    ZAP(density);
}

void rSetRotation(FAN_Hash * ret, char **params)
{
    void **data = (void **) params;

    double *rot = (double *) data[1];

    FAN_sendMessage(masterCom, "setRotation", (void *) rot);
}

void *mSetRotation(FAN_Hash * reg, void *param)
{
    double *rot = (double *) param;
    
    bool changed = true;

    if(lastRot != NULL)
    {
        changed = false;
        for(int i=0; i<3; i++)
            for(int j=0; j<3; j++)
            {
                if(NEQ(rot[i*3 + j], lastRot[i*3 + j]))
                    changed = true;
            }
    }

    ZAP_ARRAY(lastRot);
    lastRot = new double[9];
    memcpy(lastRot, rot, 9 * sizeof(double));

    if(changed)
    {
        csmdlCon->setRotationMatrix(rot);
        modelUpdated = true;
    }

    return (void *) FAN_OK;

}

void *mSimStarted(FAN_Hash * reg, void *param)
{
    sim_waitForStart = false;
    simPaused = false;
    simStarted = true;
    sendSimBound();
    sendSimStatus(true);
    return (void *) FAN_OK;
}

void rRestartSim(FAN_Hash * ret, char **params)
{
    FAN_postMessage(simMasterCom, "stop", NULL, NULL);
    FAN_postMessage(simMasterCom, "start", NULL, NULL);

    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");

}

void startSim()
{
    if (simPaused && !sim_waitForStart) {
        if (voxels == NULL || modelUpdated || modelChanged) {
            if (voxelization && modelUpdated) {
                delete voxelization;
                voxelization = NULL;
            }
            if(!FAN_sendMessage(masterCom, "updateSim", NULL) || voxels == NULL)
            {
                 return;   
            }

        }
        if (simUpdated) {
            FAN_sendMessage(simStartMasterCom, "ready", NULL);

            simulation.setAccel(stdAcceleration);
            simulation.setDensity(stdDensity);
            simulation.setRelax(stdRelaxationValue);
            simulation.setUpdateRate(simUpdateRate);

            simulation.updateVars();

            if (modelUpdated) {
                simulation.setVoxels(*voxels, simScaleX, simScaleY, simScaleZ);

            } else {
                simulation.updateVoxels(*voxels);
            }
        }

        modelUpdated = false;
        simUpdated   = false;

        FAN_postMessage(simStartMasterCom, "start", NULL, NULL);
        sim_waitForStart = true;
    }
}

void rStartSim(FAN_Hash * ret, char **params)
{
    FAN_postMessage(simMasterCom, "start", NULL, NULL);

    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");
}

void *mStartSim(FAN_Hash * reg, void *param)
{
    simFilterLoop = false;
    startSim();
    return (void *) FAN_OK;
}

void rPauseSim(FAN_Hash * ret, char **params)
{
    FAN_postMessage(simMasterCom, "pause", NULL, NULL);
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");
}

void bStopSim()
{
    if (simStarted) {
        simulation.pauseSim();
        FAN_sendMessage(simStartMasterCom, "ready", NULL);
    }
}

void bPauseSim()
{
    if (simStarted && !simPaused) {
        sim_waitForPause = true;
        simulation.pauseSim();
        FAN_sendMessage(simStartMasterCom, "ready", NULL);
    }
}

void *mPauseSim(FAN_Hash * reg, void *param)
{
    if (!sim_waitForPause && !simPaused && simStarted) {
        sim_waitForPause = true;
        simulation.pauseSim();
    }

    return (void *) FAN_OK;
}

void *mSimPaused(FAN_Hash * reg, void *param)
{
    sendSimStatus(false);

    if (sim_waitForPause) {
        simPaused = true;
        sim_waitForPause = false;
    
    } else {
        simPaused = true;
        simStarted = false;
    }

    simFilterLoop = true;
}

void rStopSim(FAN_Hash * ret, char **params)
{
    FAN_postMessage(simMasterCom, "stop", NULL, NULL);
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");
}

void *mStopSim(FAN_Hash * reg, void *param)
{
    simFilterLoop = false;

    if (simStarted) {
        simulation.pauseSim();
        simStarted = false;
    }else
    {
	    simPaused = true;
    }

    return (void *) FAN_OK;
}

void rResetMinMax(FAN_Hash * ret, char **params)
{
    FAN_postMessage(simMasterCom, "resetMinMax", NULL, NULL);
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "OK");
}

void *mResetMinMax(FAN_Hash * reg, void *param)
{
    simulation.resetMinMax();

    return (void *) FAN_OK;
}

//rNewPointSample
void rNewSample(FAN_Hash * ret, char **params)
{
    void **data = (void **) params;

    int *id = (int *) data[1];
    sample_type *type = (sample_type *) data[2];
    probe_type *ptype = (probe_type *) data[3];


    sample_desc_type *desc = new sample_desc_type();
    desc->id = *(id);
    desc->type = *(type);
    desc->ptype = *(ptype);
    switch (desc->ptype) {
    case PROBETYPE_RIBBON:
        {
            double *height = (double *) data[4];
            int *count = (int *) data[5];
            vertex_t *points = (vertex_t *) data[6];
            desc->height = *(height);
            desc->count = *(count);
            desc->points = points;
            break;
        }
    case PROBETYPE_GLYPH:
        {
            int *count = (int *) data[4];
            vertex_t *points = (vertex_t *) data[5];
            desc->count = *(count);
            desc->points = points;
            break;
        }
    default:
        vertex_t * points = (vertex_t *) data[4];
        desc->points = points;
    }

    FAN_postMessage(simMasterCom, "newSample", NULL, (void *) desc);
}


void *mNewSample(FAN_Hash * reg, void *param)
{
    sample_desc_type *desc = (sample_desc_type *) param;

    if (voxelization != NULL) {
        char *cid;
        asprintf(&cid, "%d", desc->id);

        sample_save_type *save = NULL;
        save = (sample_save_type *) samples->getPointer(cid);

        if (save == NULL) {
            save = new sample_save_type();
            save->points = NULL;
        }

        ZAP_ARRAY(save->points);
        ZAP_ARRAY(save->orig_points);

        save->id = desc->id;
        save->type = desc->type;
        save->ptype = desc->ptype;
        save->height = desc->height;
        save->count = desc->count;

        if (save->ptype == PROBETYPE_POINT) {
            NodeIndex idx = voxelization->getNodeIndex(desc->points[0].x, desc->points[0].y, desc->points[0].z);
            save->points = new vertex_t[1];
            save->points->x = (float) idx.getX();
            save->points->y = (float) idx.getY();
            save->points->z = (float) idx.getZ();
            save->orig_points = new vertex_t[1];
            save->orig_points->x = desc->points[0].x;
            save->orig_points->y = desc->points[0].y;
            save->orig_points->z = desc->points[0].z;

        } else if (save->ptype == PROBETYPE_PLANE) {
            save->points = new vertex_t[4];
            save->orig_points = new vertex_t[4];
            for (int i = 0; i < 4; i++) {
                NodeIndex idx = voxelization->getNodeIndex(desc->points[i].x, desc->points[i].y, desc->points[i].z);
                save->points[i].x = (float) idx.getX();
                save->points[i].y = (float) idx.getY();
                save->points[i].z = (float) idx.getZ();
                save->orig_points->x = desc->points[i].x;
                save->orig_points->y = desc->points[i].y;
                save->orig_points->z = desc->points[i].z;
            }
        } else if (save->ptype == PROBETYPE_VOLUME) {
            save->points = new vertex_t[8];
            save->orig_points = new vertex_t[8];
            for (int i = 0; i < 8; i++) {
                NodeIndex idx = voxelization->getNodeIndex(desc->points[i].x, desc->points[i].y, desc->points[i].z);
                save->points[i].x = (float) idx.getX();
                save->points[i].y = (float) idx.getY();
                save->points[i].z = (float) idx.getZ();
                save->orig_points->x = desc->points[i].x;
                save->orig_points->y = desc->points[i].y;
                save->orig_points->z = desc->points[i].z;

            }
        } else {
            save->count = desc->count;
            save->height = desc->height;
            save->points = new vertex_t[save->count];
            save->orig_points = new vertex_t[save->count];
            for (int i = 0; i < save->count; i++) {
                NodeIndex idx = voxelization->getNodeIndex(desc->points[i].x, desc->points[i].y, desc->points[i].z);
                save->points[i].x = (float) idx.getX();
                save->points[i].y = (float) idx.getY();
                save->points[i].z = (float) idx.getZ();
                save->orig_points[i].x = desc->points[i].x;
                save->orig_points[i].y = desc->points[i].y;
                save->orig_points[i].z = desc->points[i].z;

            }
        }

        samples->insertPointer(cid, save);

        if(simFilterLoop)
        {
            FAN_postMessage(simStartMasterCom, "filterLoop", NULL, cid);    
        }
    }
    delete desc;

    return (void *) FAN_OK;
}

void rDeleteSample(FAN_Hash * ret, char **params)
{
    char *id = NULL;

    FAN_aGetParam(params, &id, 0);
    if (id != NULL) {
        FAN_postMessage(simMasterCom, "deleteSample", NULL, id);

        ret->insert("RETURN", "true");
        ret->insert("RETURNMSG", "done");
    } else {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "missing id");
    }
}

void *mClearSamples(FAN_Hash * reg, void *param)
{
    int size = samples->getPointerLength();
    char **list = (char **) malloc(size * sizeof(char *));

    samples->getPointerKeys(list);

    for (int i = 0; i < size; i++) {
        char *key = *(list + i);

        sample_save_type *sample = (sample_save_type *) samples->getPointer(key);
        if (sample) {
            ZAP_ARRAY(sample->points);
            ZAP_ARRAY(sample->orig_points);
            ZAP(sample);
            samples->insertPointer(key, NULL);
        }
    }

    MZAP(list);
    return (void *) FAN_OK;
}

void *mDeleteSample(FAN_Hash * reg, void *param)
{
    char *id = (char *) param;
    sample_save_type *sample = (sample_save_type *) samples->getPointer(id);
    if (sample) {
        ZAP_ARRAY(sample->points);
        ZAP_ARRAY(sample->orig_points);
        ZAP(sample);
        samples->insertPointer(id, NULL);
    }
    MZAP(id);

    return (void *) FAN_OK;
}

void sendPointSample(FAN_Connection * conn, point_sample * sample)
{
    char *templ;
    asprintf(&templ, "int;int;int;int;double;double;{double}[1]");
    conn->binaryPush(templ, sample->id, PROBETYPE_POINT, sample->type, sample->dimensionality, sample->min, sample->max, sample->value);
    MZAP(templ);
}

void sendPlanarSample(FAN_Connection * conn, planar_sample * sample)
{
    char *templ;
    asprintf(&templ, "int;int;int;int;double;double;int;int;{double}[%d]", sample->u_size * sample->v_size);
    conn->binaryPush(templ, sample->id, PROBETYPE_PLANE, sample->type, sample->dimensionality, sample->min, sample->max, sample->u_size, sample->v_size, sample->values);
    MZAP(templ);
}

void sendVolumeSample(FAN_Connection * conn, volume_sample * sample)
{
    char *templ;
    asprintf(&templ, "int;int;int;int;double;double;{double}[%d]", sample->u_size * sample->v_size * sample->w_size);
    conn->binaryPush(templ, sample->id, PROBETYPE_PLANE, sample->type, sample->dimensionality, sample->min, sample->max, sample->u_size, sample->v_size, sample->w_size, sample->values);
    MZAP(templ);
}

void sendGlyphSample(FAN_Connection * conn, glyph_probe_data * sample)
{
    char *templ;
    asprintf(&templ, "int;int;int;double;double;{int}[%d];{double;double;double}[%d];{double;double;double}[%d];{double}[%d]", sample->num_streams, sample->num_total, sample->num_total, sample->num_total);
    conn->binaryPush(templ, sample->id, PROBETYPE_GLYPH, sample->num_streams, sample->min_value, sample->max_value, sample->num_glyphs_per_stream, sample->positions, sample->directions, sample->values);
    MZAP(templ);
}

void sendRibbonSample(FAN_Connection * conn, ribbon_probe_data * sample)
{
    char *templ;
    asprintf(&templ, "int;int;int;double;double;int;{int}[%d];{double;double;double}[%d];{double}[%d]", sample->num_ribbons, sample->num_total * 2, sample->num_total);
    conn->binaryPush(templ, sample->id, PROBETYPE_RIBBON, sample->num_ribbons, sample->min_value, sample->max_value, sample->num_total, sample->num_vertices, sample->ribbons, sample->values);
    MZAP(templ);
}

void freePointSample(point_sample * sample)
{
    ZAP(sample);
}

void freePlanarSample(planar_sample * sample)
{
    ZAP_ARRAY(sample->values);
    ZAP(sample);
}

void freeVolumeSample(volume_sample * sample)
{
    ZAP_ARRAY(sample->values);
    ZAP(sample);
}

void freeGlyphSample(glyph_probe_data * sample)
{
    ZAP_ARRAY(sample->positions);
    ZAP_ARRAY(sample->directions);
    ZAP_ARRAY(sample->values);
    ZAP(sample);
}

void freeRibbonSample(ribbon_probe_data * sample)
{
    ZAP_ARRAY(sample->ribbons);
    ZAP_ARRAY(sample->values);
    ZAP(sample);
}

void *mSimReady(FAN_Hash *reg, void *p)
{
    return (void *) FAN_OK;
}

void *mSimFilterLoop(FAN_Hash * reg, void *p)
{
    char *cid = (char*)p;
    simulation.filterInit();
    FAN_sendMessage(simMasterCom, "filterSample", cid);
    simulation.filterDone();
    return (void *) FAN_OK;
}

void *mSimFilterSample(FAN_Hash * reg, void *p)
{
    char *cid = (char*)p;

    if (visConn2 != NULL) {
        visConn2->startBinaryPush("vis::putSample");

        sample_save_type *sample_desc = (sample_save_type *) samples->getPointer(cid);

        switch (sample_desc->ptype) {
        case PROBETYPE_POINT:
           {
                point_sample *sample = simulation.computePointSample(sample_desc);
                sendPointSample(visConn2, sample);
                freePointSample(sample);
                break;
           }
        case PROBETYPE_PLANE:
           {
               planar_sample *sample = simulation.computePlanarSample(sample_desc);
               sendPlanarSample(visConn2, sample);
               freePlanarSample(sample);
               break;
           }
        case PROBETYPE_VOLUME:
           {
               volume_sample *sample = simulation.computeVolumeSample(sample_desc);
               sendVolumeSample(visConn2, sample);
               freeVolumeSample(sample);
               break;
           }
        case PROBETYPE_GLYPH:
           {
               glyph_probe_data *sample = simulation.computeGlyphSample(sample_desc);
               sendGlyphSample(visConn2, sample);
               freeGlyphSample(sample);
               break;
           }
        case PROBETYPE_RIBBON:
           {
               ribbon_probe_data *sample = simulation.computeRibbonSample(sample_desc, g_voxelSize);
               sendRibbonSample(visConn2, sample);
               freeRibbonSample(sample);
               break;
           }
        }
        visConn2->stopBinaryPush();
    }

    return (void *) FAN_OK;
}



void *mSimFilter(FAN_Hash * reg, void *p)
{
    int steps = (int) p;

    if (visConn2 == NULL) {
        mClearSamples(reg, NULL);
    }
    if (visConn2 != NULL && steps != 0) {
        char *csteps;
        asprintf(&csteps, "%d", steps);
        printf("steps: %s, %d\n", csteps, steps);
        visConn2->rpc("vis::setSteps", 1, csteps);
        MZAP(csteps);
    }

    int size = samples->getPointerLength();
    if (size > 0) {
        void **list = (void **) malloc(size * sizeof(void *));
        samples->getPointerKeys((char **) list);
        if (visConn2 != NULL) {
            visConn2->startBinaryPush("vis::putSample");
            for (int i = 0; i < size; i++) {
                sample_save_type *sample_desc = (sample_save_type *) samples->getPointer((char *) *(list + i));

                switch (sample_desc->ptype) {
                case PROBETYPE_POINT:
                    {
                        point_sample *sample = simulation.computePointSample(sample_desc);
                        sendPointSample(visConn2, sample);
                        freePointSample(sample);
                        break;
                    }
                case PROBETYPE_PLANE:
                    {
                        planar_sample *sample = simulation.computePlanarSample(sample_desc);
                        sendPlanarSample(visConn2, sample);
                        freePlanarSample(sample);
                        break;
                    }
                case PROBETYPE_VOLUME:
                    {
                        volume_sample *sample = simulation.computeVolumeSample(sample_desc);
                        sendVolumeSample(visConn2, sample);
                        freeVolumeSample(sample);
                        break;
                    }
                case PROBETYPE_GLYPH:
                    {
                        glyph_probe_data *sample = simulation.computeGlyphSample(sample_desc);
                        sendGlyphSample(visConn2, sample);
                        freeGlyphSample(sample);
                        break;
                    }
                case PROBETYPE_RIBBON:
                    {
                        ribbon_probe_data *sample = simulation.computeRibbonSample(sample_desc, g_voxelSize);
                        sendRibbonSample(visConn2, sample);
                        freeRibbonSample(sample);
                        break;
                    }
                }
            }
            visConn2->stopBinaryPush();
        }
        MZAP(list);
    }


    return (void *) FAN_OK;
}


void rSaveAs(FAN_Hash * ret, char **params)
{
    char *filename = NULL;

    FAN_aGetParam(params, &filename, 0);

    FAN_postMessage(masterCom, "saveAs", NULL, filename);

    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "Saving of CAD-file successfully queued");
}

void *mSaveAs(FAN_Hash * reg, void *p)
{
    char *file = (char *) p;

    char *filename = NULL;
    if (file) {
        asprintf(&filename, "%s%s", g_fan->config->getValue("modelpath", "../data/test_models/"), (char *) file);
        MZAP(file);
    }

    if (csmdlCon != NULL) {
        if (filename == NULL) {
            csmdlCon->storeCADFile();
        } else {
            csmdlCon->storeCADFileAs(filename);
            MZAP(filename);
        }
    }

    return (void *) FAN_OK;
}

void rGetModelList(FAN_Hash * ret, char **params)
{
    DIR *dir_p;
    struct dirent *dir_entry_p;
    int count = 0;

    dir_p = opendir(g_fan->config->getValue("modelpath", "../data/test_models"));
    while (NULL != (dir_entry_p = readdir(dir_p))) {
        char *name = dir_entry_p->d_name;
        if (strstr(name, ".stp") != NULL || strstr(name, ".step") != NULL || strstr(name, ".wrl") != NULL || strstr(name, ".iges") != NULL || strstr(name, ".stl") != NULL || strstr(name, ".brep") != NULL) {
            char *idx;
            asprintf(&idx, "file%d", count++);
            ret->insert(idx, name);
            MZAP(idx);
        }
    }
    char *ccount;
    asprintf(&ccount, "%d", count);
    ret->insert("count", ccount);
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "ok");
    MZAP(ccount);
    closedir(dir_p);
}

void sendFace(FAN_Connection * conn, FACE_TRGL current_face, int id)
{
    triangle_t *triangles = new triangle_t[current_face.iSizeTriangles];

    for (int j = 0; j < current_face.iSizeTriangles; j++) {
        // open cascade arrays start at 1...
        triangles[j].a = current_face.triangles[j].a - 1;
        triangles[j].b = current_face.triangles[j].b - 1;
        triangles[j].c = current_face.triangles[j].c - 1;
    }

    if (conn != NULL) {
        char *templ = NULL;
        asprintf(&templ, "int;int;int;{double;double;double}[%d];{int;int;int}[%d];int;int;{double;double;double}[%d]", current_face.iSizeVertices, current_face.iSizeTriangles, current_face.iSizePolesU * current_face.iSizePolesV);

        conn->binaryPush(templ, id, current_face.iSizeVertices, current_face.iSizeTriangles, current_face.vertices, triangles, current_face.iSizePolesU, current_face.iSizePolesV, current_face.poles);
        MZAP(templ);
    }

    delete[]triangles;
}

void sendFaces(FAN_Connection * conn, FACE_TRGL * myFaces, int size)
{
    for (int i = 0; i < size; i++) {
        FACE_TRGL current_face = myFaces[i];

        sendFace(conn, current_face, i);

    }
}

void startData(FAN_Connection * conn, int size)
{
    if (conn != NULL) {
        char *csize = NULL;
        asprintf(&csize, "%d", size);
        conn->rpc("vis::startData", csize);
        MZAP(csize);
        FAN_xlog(FAN_ERROR, "StartData");

        conn->startBinaryPush("vis::putData");
    }
}

void stopData(FAN_Connection * conn)
{
    if (conn != NULL) {
        conn->stopBinaryPush();

        conn->rpc("vis::stopData");
        FAN_xlog(FAN_ERROR, "StopData");
    }
}

typedef struct changes2_t {
    int idxFace;
    vertex_t *control_points;
};

void *mChangePoles(FAN_Hash * reg, void *data);
void rChangePoles(FAN_Hash * ret, char **params)
{
    int *idxFace = NULL;
    vertex_t *control_points = NULL;

    void **data = (void **) params;

    idxFace = (int *) data[1];
    control_points = (vertex_t *) data[2];

    changes2_t *changes = new changes2_t();
    changes->idxFace = (int) (*idxFace);
    changes->control_points = control_points;

    FAN_postMessage(masterCom, "changePoles", NULL, (void *) changes);
}

void *mChangePoles(FAN_Hash * reg, void *param) // int idxFace, int idxU, int idxV, CSPOLE nPoint)
{
    changes2_t *changes = (changes2_t *) param;
    int idxFace = changes->idxFace;
    CSPOLE *nPoint = (CSPOLE *) changes->control_points;

    if (csmdlCon != NULL) {
        FACE_TRGL changedFace;
        bool isChanged, bSuccess = false;

        //set new position of control point
        isChanged = csmdlCon->changePoles(idxFace, nPoint);
        if (isChanged) {
            bPauseSim();

            //get new triangulated face
            changedFace = csmdlCon->getTriangulationOfFace(idxFace, bSuccess);
            if (bSuccess) {
                if (visConn != NULL) {
                    visConn->startBinaryPush("vis::putData");

                    sendFace(visConn, changedFace, idxFace);

                    visConn->stopBinaryPush();
                }
                modelChanged = true;
            } else
                FAN_xlog(FAN_ERROR, "Could not triangulate face!");

            FAN_postMessage(simMasterCom, "start", NULL, NULL);
        } else
            FAN_xlog(FAN_ERROR, "Could not change position of new control Point!");

    }
    delete changes;
    return NULL;
}

typedef struct changes_t {
    int idxFace;
    int idxU;
    int idxV;
    double x;
    double y;
    double z;
};

void rChangePole(FAN_Hash * ret, char **params)
{
    char *idxFace = NULL;
    char *idxU = NULL;
    char *idxV = NULL;
    char *x = NULL;
    char *y = NULL;
    char *z = NULL;

    FAN_aGetParam(params, &idxFace, 0);
    FAN_aGetParam(params, &idxU, 1);
    FAN_aGetParam(params, &idxV, 2);
    FAN_aGetParam(params, &x, 3);
    FAN_aGetParam(params, &y, 4);
    FAN_aGetParam(params, &z, 5);

    if (idxFace == NULL || idxU == NULL || idxV == NULL || x == NULL || y == NULL || z == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Wrong parameter(s) - Not a valid Filename");
    } else {
        changes_t *changes = new changes_t();
        changes->idxFace = atoi(idxFace);
        changes->idxU = atoi(idxU);
        changes->idxV = atoi(idxV);
        changes->x = atof(x);
        changes->y = atof(y);
        changes->z = atof(z);

        FAN_postMessage(masterCom, "changePole", NULL, (void *) changes);

        ret->insert("RETURN", "true");
        ret->insert("RETURNMSG", "OK");
    }

    // Cleanup
    MZAP(idxFace);
    MZAP(idxV);
    MZAP(idxU);
    MZAP(x);
    MZAP(y);
    MZAP(z);
}

void *mChangePole(FAN_Hash * reg, void *param)  // int idxFace, int idxU, int idxV, CSPOLE nPoint) 
{
    changes_t *changes = (changes_t *) param;
    int idxFace = changes->idxFace;
    int idxU = changes->idxU;
    int idxV = changes->idxV;
    CSPOLE nPoint;
    nPoint.x = changes->x;
    nPoint.y = changes->y;
    nPoint.z = changes->z;

    if (csmdlCon != NULL) {
        FACE_TRGL changedFace;
        bool isChanged, bSuccess = false;
        FAN_xlog(FAN_ERROR, "LOG: %d,%d,%d", idxFace, idxU, idxV);
        //set new position of control point
        isChanged = csmdlCon->changePole(idxFace, idxU, idxV, nPoint);
        if (isChanged) {
            bPauseSim();

            //get new triangulated face
            changedFace = csmdlCon->getTriangulationOfFace(idxFace, bSuccess);
            if (bSuccess) {
                if (visConn != NULL) {
                    visConn->startBinaryPush("vis::putData");

                    sendFace(visConn, changedFace, idxFace);

                    visConn->stopBinaryPush();

                    setServerStatus(visConn, "", "-1");
                }
                modelChanged = true;
            } else
                FAN_xlog(FAN_ERROR, "Could not triangulate face!");

            // startSim();

        } else
            FAN_xlog(FAN_ERROR, "Could not change position of new control Point!");

    }
    delete changes;
    return NULL;
}

void rSetDeflection(FAN_Hash * ret, char **params)
{
    char *defl = NULL;
    double dbDefl = 0;

    FAN_aGetParam(params, &defl, 0);

    if (csmdlCon == NULL || defl == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Missing parameter [defl]");
    } else {
        dbDefl = atof(defl);
        if (dbDefl > 0 && csmdlCon->setDeflection(dbDefl)) {
            ret->insert("RETURN", "true");
            ret->insert("RETURNMSG", "OK");
        } else {
            ret->insert("RETURN", "false");
            ret->insert("RETURNMSG", "DANGER");
        }
    }
}

void rLoadFile(FAN_Hash * ret, char **params)
{
    char *name = NULL;

    FAN_aGetParam(params, &name, 0);

    if (name == NULL) {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Wrong parameter(s) - Not a valid Filename");
    } else {
        FAN_xlog(FAN_INFO | FAN_MISC, "Loading file %s", name);
        FAN_postMessage(masterCom, "loadFile", NULL, (void *) name);

        ret->insert("RETURN", "true");
        ret->insert("RETURNMSG", "OK");
    }
}

void *mLoadFile(FAN_Hash * reg, void *file)
{
    char *filename;
    asprintf(&filename, "%s%s", g_fan->config->getValue("modelpath", "../data/test_models/"), (char *) file);
    MZAP(file);

    FAN_postMessage(simMasterCom, "clearSamples", NULL, NULL);

    FACE_TRGL *myFaces;

    if (csmdlCon != NULL) {
        bStopSim();

        setServerStatus(visConn, "Loading model", "0");

        cout << "Before Loading File" << endl;

        if (csmdlCon->setCADFilename(filename)) {
            cout << "After Loading File" << endl;
            setServerStatus(visConn, "Triangulating model", "0");

            int size = 0;
            myFaces = csmdlCon->getTriangulation(size);

            startData(visConn, size);

            sendFaces(visConn, myFaces, size);

            stopData(visConn);

            if (voxelization != NULL) {
                delete voxelization;
                voxelization = NULL;
            }

            modelUpdated = true;
        } else {
            setServerStatus(visConn, "Loading model failed !", "0");
        }
    }

    MZAP(filename);

    return NULL;
}

void *mSimRunner(FAN_Hash * reg, void *p)
{
    simulation.startSim();
    return (void *) FAN_OK;
}

int main(int argc, char **argv)
{
    //Init MPI
    MPI::Init_thread(MPI_THREAD_MULTIPLE);

    g_fan = new FAN("../etc/model.conf", false);
    g_fan->installIntHandler();
    FAN_init();

    FAN_createDomain("model", true);
    FAN_loadExtCmd("model::login", &rLogin, true, true);
    FAN_loadExtCmd("model::logout", &rLogout, true, true);
    FAN_loadExtCmd("model::loadFile", &rLoadFile, true, true);
    FAN_loadExtCmd("model::saveAs", &rSaveAs, true, true);
    FAN_loadExtCmd("model::getModelList", &rGetModelList, true, true);
    FAN_loadExtCmd("model::changePole", &rChangePole, true, true);
    FAN_loadExtCmd("model::changePoles", &rChangePoles, true, true);
    FAN_loadExtCmd("model::setDeflection", &rSetDeflection, true, true);
    FAN_loadExtCmd("model::setRotation", &rSetRotation, true, true);
    FAN_loadExtCmd("model::setVoxelRes", &rSetVoxelRes, true, true);

    FAN_loadExtCmd("sim::start", &rStartSim, true, true);
    FAN_loadExtCmd("sim::stop", &rStopSim, true, true);
    FAN_loadExtCmd("sim::pause", &rPauseSim, true, true);
    FAN_loadExtCmd("sim::resetMinMax", &rResetMinMax, true, true);
    FAN_loadExtCmd("sim::setDensity", &rSetDensity, true, true);
    FAN_loadExtCmd("sim::setRelaxationValue", &rSetRelaxationValue, true, true);
    FAN_loadExtCmd("sim::setAcceleration", &rSetAcceleration, true, true);
    FAN_loadExtCmd("sim::setSimScale", &rSetSimScale, true, true);
    FAN_loadExtCmd("sim::setUpdateRate", &rSetUpdateRate, true, true);
    FAN_loadExtCmd("sim::newSample", &rNewSample, true, true);
    FAN_loadExtCmd("sim::deleteSample", &rDeleteSample, true, true);
    FAN_loadExtCmd("sim::restart", &rRestartSim, true, true);

    FAN_ThreadedDaemon *d = new FAN_ThreadedDaemon(NULL, "modelbindhost", "modelport");

    visBuffer = FAN::app->config->getValue("VISBUFFER", "1000000");
    if (argc >= 2)
        FAN::app->config->insert("VISHOST", argv[2]);
    visHost = strdup(FAN::app->config->getValue("VISHOST", "127.0.0.1"));
    if (argc >= 3)
        FAN::app->config->insert("VISPORT", argv[3]);
    visPort = atoi(FAN::app->config->getValue("VISPORT", "30001"));
    if (argc >= 1)
        FAN::app->config->insert("MODELPORT", argv[1]);

    FAN_Hash *master = FAN_initMasterHandler();
    FAN_registerHandler(master, "checkLogin", &mCheckLogin);
    FAN_registerHandler(master, "login", &mLogin);
    FAN_registerHandler(master, "logout", &mLogout);
    FAN_registerHandler(master, "saveAs", &mSaveAs);
    FAN_registerHandler(master, "loadFile", &mLoadFile);
    FAN_registerHandler(master, "changePole", &mChangePole);
    FAN_registerHandler(master, "changePoles", &mChangePoles);
    FAN_registerHandler(master, "setRotation", &mSetRotation);
    FAN_registerHandler(master, "updateSim", &mUpdateSim);
    masterCom = (FAN_Com *) master->getPointer("COM");

    FAN_Hash *simMaster = FAN_initMasterHandler();
    FAN_registerHandler(simMaster, "login", &mSimLogin);
    FAN_registerHandler(simMaster, "logout", &mSimLogout);
    FAN_registerHandler(simMaster, "filter", &mSimFilter);
    FAN_registerHandler(simMaster, "filterSample", &mSimFilterSample);
    FAN_registerHandler(simMaster, "stop", &mStopSim);
    FAN_registerHandler(simMaster, "pause", &mPauseSim);
    FAN_registerHandler(simMaster, "start", &mStartSim);
    FAN_registerHandler(simMaster, "paused", &mSimPaused);
    FAN_registerHandler(simMaster, "started", &mSimStarted);
    FAN_registerHandler(simMaster, "resetMinMax", &mResetMinMax);
    FAN_registerHandler(simMaster, "newSample", &mNewSample);
    FAN_registerHandler(simMaster, "deleteSample", &mDeleteSample);
    FAN_registerHandler(simMaster, "clearSamples", &mClearSamples);
    simMasterCom = (FAN_Com *) simMaster->getPointer("COM");

    FAN_Hash *simStartMaster = FAN_initMasterHandler();
    FAN_registerHandler(simStartMaster, "start", &mSimRunner);
    FAN_registerHandler(simStartMaster, "filterLoop", &mSimFilterLoop);
    FAN_registerHandler(simStartMaster, "ready", &mSimReady);
    simStartMasterCom = (FAN_Com *) simStartMaster->getPointer("COM");


    initController();

    while (!d->bindDaemon()) {
        FAN_wait(2, 0);
        FAN_xlog(FAN_DEBUG | FAN_SOCKET, "Retry");
    }

    cout << "Send SHUTDOWN Message to MPI-Clients ...";
    simulation.haltSim();

    if (simMasterCom) {
        delete simMasterCom;
        simMasterCom = NULL;
    }
    if (masterCom) {
        delete masterCom;
        masterCom = NULL;
    }

    delete d;
    delete g_fan;

    MPI::Finalize();
    return 0;
}
