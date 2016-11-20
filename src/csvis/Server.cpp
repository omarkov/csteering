// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org> 
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

#include "Application.h"
#include "simRemoteTypes.h"
#include "TriangulatedModel.h"
#include "CommonServer.h"
#include "Server.h"
#include "LoginWindow.h"
#include "GUIController.h"
#include "ProbeManager.h"

FAN *g_fan = NULL;
osg::ref_ptr<TriangulatedModel> g_model = NULL;
int g_current_face = 0;
int g_num_faces = 0;

void rPutSample(FAN_Hash *ret, char **params)
{
    void      **data   = (void**)params;
    int        *id     = (int*)        data[1];
    probe_type ptype   = (probe_type)*(probe_type*) data[2];

    if(ptype == PROBETYPE_POINT)
    {
	data_type  *type   = (data_type*)  data[3];
    	int        *dim    = (int*)        data[4];
    	double     *min    = (double*)     data[5];
    	double     *max    = (double*)     data[6];
    	double     *values = (double*)     data[7];

        point_sample *sample   = new point_sample();
	sample->id             = *(id);
	sample->type           = *(type);
	sample->dimensionality = (unsigned int)*(dim);
	sample->min            = *(min);
	sample->max            = *(max);
	sample->value          = (double*)values;  
	delete sample;
    }else if(ptype == PROBETYPE_PLANE)
    {
	data_type  *type   = (data_type*)  data[3];
    	int        *dim    = (int*)        data[4];
    	double     *min    = (double*)     data[5];
    	double     *max    = (double*)     data[6];
    	int        *u_size = (int*)        data[7];
    	int        *v_size = (int*)        data[8];
    	double     *values = (double*)     data[9];

        planar_sample *sample  = new planar_sample();
	sample->id             = (int)*id;
	sample->type           = (data_type)*type;
	sample->dimensionality = (unsigned int)*dim;
	sample->min            = (double)*min;
	sample->max            = (double)*max;
	sample->u_size         = (int)*u_size;
	sample->v_size         = (int)*v_size;
	sample->values = new double[sample->u_size*sample->v_size];
	memcpy(sample->values, values, sample->u_size*sample->v_size*sizeof(double));

	g_ProbeManager->updateSliceProbe(sample);
    }else if(ptype == PROBETYPE_GLYPH)
    {
	int *num_streams   = (int*)          data[3];
    	double   *min        = (double*)     data[4];
    	double   *max        = (double*)     data[5];
        int *num_glyphs_per_stream = (int*)  data[6];
    	vertex_t *positions  = (vertex_t*)   data[7];
    	vertex_t *directions = (vertex_t*)   data[8];
    	double   *values     = (double*)     data[9];

        glyph_probe_data *sample  = new glyph_probe_data();  
        sample->id             = (int)*id;                    
        sample->num_streams    = (int)*num_streams;            
        sample->num_glyphs_per_stream = (unsigned int*)num_glyphs_per_stream; 

        sample->min_value      = (double)*min;                     
        sample->max_value      = (double)*max;                    
        sample->positions      = (vertex_t*)positions;               
        sample->directions     = (vertex_t*)directions;            
        sample->values         = (double*)values;                

        g_ProbeManager->updateGlyphProbe(sample);
    }else if(ptype == PROBETYPE_RIBBON)
    {
        int *num_ribbons     = (int*)        data[3];
    	double   *min        = (double*)     data[4];
    	double   *max        = (double*)     data[5];
	int 	 total       = *(int*)       data[6];
        int *num_vertices    = (int*)        data[7];
    	vertex_t *ribbons    = (vertex_t*)   data[8];
    	double   *values     = (double*)     data[9];


        ribbon_probe_data *sample  = new ribbon_probe_data();
	memcpy(&sample->id, id, sizeof(int));
	memcpy(&sample->num_ribbons, num_ribbons, sizeof(int));
	memcpy(&sample->min_value, min, sizeof(double));
	memcpy(&sample->max_value, max, sizeof(double));
        sample->num_vertices   = new unsigned int[sample->num_ribbons];
	memcpy(sample->num_vertices, num_vertices, sample->num_ribbons*sizeof(unsigned int));
        sample->ribbons        = new vertex_t[total*2];
	memcpy(sample->ribbons, ribbons, total*2*sizeof(vertex_t));
        sample->values         = new double[total];
	memcpy(sample->values, values, total*sizeof(double));

        g_ProbeManager->updateRibbonProbe(sample);
    }else if(ptype == PROBETYPE_VOLUME)
    {
	data_type  *type   = (data_type*)  data[3];
    	int        *dim    = (int*)        data[4];
    	double     *min    = (double*)     data[5];
    	double     *max    = (double*)     data[6];
    	int        *u_size = (int*)        data[7];
    	int        *v_size = (int*)        data[8];
    	int        *w_size = (int*)        data[9];
    	double     *values = (double*)     data[10];

        volume_sample *sample  = new volume_sample();
	sample->id             = (int)*id;
	sample->type           = (data_type)*type;
	sample->dimensionality = (unsigned int)*dim;
	sample->min            = (double)*min;
	sample->max            = (double)*max;
	sample->u_size         = (int)*u_size;
	sample->v_size         = (int)*v_size;
	sample->w_size         = (int)*w_size;
	sample->values         = (double*)values;  
	delete sample;
    }
}

typedef struct msg_t
{
  face_t *face;
};

void rSetSimStatus(FAN_Hash *ret, char **params)
{
    char *status = NULL;
    FAN_aGetParam(params, &status, 0); 

    if(status)
    {
	bool bStatus = atoi(status);
        g_GUIController->osd()->setSimulationRunning(bStatus);
        
	ret->insert("RETURN", "true");
	ret->insert("RETURNMSG", "OK");
	free(status);
    }else
    {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Invalid Parameter");
    }

}

void rSetSteps(FAN_Hash *ret, char **params)
{
    char *steps = NULL;
    FAN_aGetParam(params, &steps, 0); 

    if(steps != NULL)
    {
        int isteps = atoi(steps);

        g_GUIController->osd()->setSimulationSteps(isteps);
        
	ret->insert("RETURN", "true");
	ret->insert("RETURNMSG", "OK");
	free(steps);
    }else
    {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Invalid Parameter");
    }

}

void rLoadFile(FAN_Hash *ret, char **params)
{
    char *file = NULL;
    FAN_aGetParam(params, &file, 0); 

    if(file != NULL)
    {
        if(modelConn != NULL)
	{
            modelConn->rpc("model::loadFile", 1, file);

	    ret->insert("RETURN", modelConn->returnHash->getValue("RETURN"));
	    ret->insert("RETURNMSG", modelConn->returnHash->getValue("RETURNMSG"));
	}else
	{
	    ret->insert("RETURN", "false");
	    ret->insert("RETURNMSG", "Not connected");
	}
	free(file);
    }else
    {
        ret->insert("RETURN", "false");
        ret->insert("RETURNMSG", "Invalid Parameter");
    }
}
void rStartData(FAN_Hash *ret, char **params)
{
    char *num_faces = NULL;
    FAN_aGetParam(params, &num_faces, 0);
    g_num_faces = strtol((char*)num_faces, (char**)NULL, 10);
    g_current_face = 0;

    FAN_xlog(FAN_ERROR, "START DATA"); 
    FAN_sendMessage(masterCom, "startData", NULL);
    free(num_faces);
    ret->insert("RETURN", "true");
    ret->insert("RETURNMSG", "Waiting for new data");
}
void *mStartData(FAN_Hash *reg, void *data)
{
//    g_Application->clearScene();
    g_model = new TriangulatedModel();
    return (void*)OK;
}

void rSetSimulationBound(FAN_Hash *ret, char **params)
{
    char *cx = NULL;
    char *cy = NULL;
    char *cz = NULL;
    FAN_aGetParam(params, &cx, 0);
    FAN_aGetParam(params, &cy, 1);
    FAN_aGetParam(params, &cz, 2);

    if(cx && cy && cz )
    { 
        g_Application->simulationSpace()->setDimensions(strtod(cx, NULL), strtod(cy, NULL), strtod(cz, NULL));
    }

    if(cx) free(cx);
    if(cy) free(cy);
    if(cz) free(cz);
}
 

void rSetServerStatus(FAN_Hash *ret, char **params)
{
    char *status_text = NULL;
    FAN_aGetParam(params, &status_text, 0);

    char *status_progress = NULL;
    FAN_aGetParam(params, &status_progress, 1);

    if(status_text && status_progress)
    {
    	g_GUIController->osd()->showServerStatus(status_text, atoi(status_progress));
    }else
    {
    	g_GUIController->osd()->showServerStatus("", 0);
    }
    if(status_text) free(status_text);
    if(status_progress) free(status_progress);
}

void *mPutData(FAN_Hash *reg, void *data);
void rPutData(FAN_Hash *ret, char **params)
{

    void **data = (void**)params;
    if(g_model != NULL)
    {
        int *id = NULL;
        int *num_vertices        = NULL;
        int *num_triangles       = NULL;
        vertex_t *vertices       = NULL;
        triangle_t *triangles    = NULL;
        vertex_t *control_points = NULL;
        int *u_size              = NULL;
        int *v_size              = NULL;
        face_t *face             = new face_t;

        id             = (int*)data[1];
        num_vertices   = (int*)data[2];
        num_triangles  = (int*)data[3];
        vertices       = (vertex_t*)data[4];
        triangles      = (triangle_t*)data[5];
        u_size         = (int*)data[6];
        v_size         = (int*)data[7];
	control_points = (vertex_t*)data[8];

/*
        face->id             = *(id);
        face->num_vertices   = *(num_vertices);
        face->num_triangles  = *(num_triangles);
        face->u_size         = *(u_size);
        face->v_size         = *(v_size);
*/
        memcpy(&face->id, id, sizeof(int));
        memcpy(&face->num_vertices, num_vertices, sizeof(int));
        memcpy(&face->num_triangles, num_triangles, sizeof(int));
        memcpy(&face->u_size, u_size, sizeof(int));
        memcpy(&face->v_size, v_size, sizeof(int));

#ifdef __IRIX__
  	face->triangles     =(triangle_t*)malloc(face->num_triangles*sizeof(triangle_t));
  	face->vertices      =(vertex_t*)  malloc(face->num_vertices*sizeof(vertex_t));
  	face->control_points=(vertex_t*)  malloc(face->u_size*face->v_size*sizeof(vertex_t));
        face->triangles     =(triangle_t*)memcpy(face->triangles,triangles,face->num_triangles*sizeof(triangle_t));
        face->vertices      =(vertex_t*)  memcpy(face->vertices,vertices,face->num_vertices*sizeof(vertex_t));
        face->control_points=(vertex_t*)  memcpy(face->control_points,control_points,face->u_size*face->v_size*sizeof(vertex_t));
#else
        face->triangles      = (triangle_t*)triangles;
 	face->vertices	     = (vertex_t*)vertices;
 	face->control_points = (vertex_t*)control_points;
#endif

	msg_t *msg  = new msg_t;
	msg->face   = face;

//	mPutData(NULL, (void*)msg);
//	FAN_postMessage(masterCom, "putData", NULL, (void*)msg);
	FAN_sendMessage(masterCom, "putData", (void*)msg);
    }
}

void *mPutData(FAN_Hash *reg, void *data)
{
    char *status; 
    asprintf(&status, "Transmitting model %d/%d", g_current_face, g_num_faces); 
    g_GUIController->osd()->showServerStatus(status, (int)((float) g_current_face / (float) g_num_faces * 100.0f));
    free(status);

    g_current_face ++;

    msg_t *msg = (msg_t*)data;
    g_model->updateFace(*msg->face);
//    usleep(50000);
#ifdef __IRIX__
    free(msg->face->vertices);
    free(msg->face->triangles);
    free(msg->face->control_points);
#endif
    delete msg->face;
    delete msg;

//    g_Application->activateScene();
    return (void*)OK;
}

void *mStopData(FAN_Hash *reg, void *data)
{
    if(g_model != NULL)
    {
    	g_GUIController->osd()->showServerStatus("", -1);
        g_Application->loadModel(g_model);
	g_Application->activateScene();
        g_model->enableCallbacks();
        g_Application->model()->rescale();
	return (void*)OK;
    }else
    {
	return (void*)FAILED;
    }
}

void rStopData(FAN_Hash *ret, char **params)
{
    if(FAN_sendMessage(masterCom, "stopData", NULL) == FAN_OK)
    {
        ret->insert("RETURN", "true");
	ret->insert("RETURNMSG", "Data successfully activated");
    }else
    {
        ret->insert("RETURN", "false");
	ret->insert("RETURNMSG", "stopData not initialized. Call startData first!!");
    }
    FAN_xlog(FAN_ERROR, "STOP DATA"); 
}

void *mLogin(FAN_Hash *ret, void *data)
{
    // FIXME: 
    g_model = new TriangulatedModel();
    g_Application->loadModel(g_model);
    g_Application->activateScene();

    g_LoginWindow->showWindow();

    return (void*)OK;
}
