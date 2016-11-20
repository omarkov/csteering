#ifndef TYPES_H
#define TYPES_H

#define V0 	0
#define V1 	1
#define V2 	2
#define V3	3 
#define V4	4 
#define V5	5 
#define V6	6
#define V7	7
#define V8	8
#define V9	9
#define V10	10 
#define V11	11 
#define V12	12 
#define V13	13 
#define V14	14 
#define V15	15 
#define V16	16 
#define V17	17 
#define V18	18 

#define B1	0
#define B7	1
#define B9	2	
#define B11	3
#define B13	4

#define B2	0
#define B8	1
#define B10	2	
#define B12	3
#define B14	4

#include "../common/simRemoteTypes.h"
struct cellData
{
	double density;
	double distributionValue[19];
	double mv_x, mv_y, mv_z;
	bool solid;
};

struct bufferData
{
	double distributionValue[5];
};



struct ribbon_data
{
	float x, y, z;	
	double mv_x, mv_y, mv_z;	
};

typedef vector<ribbon_data> ribbon_t; 
typedef vector<vector<vector<bool> > > Voxels;


struct middens
{
	double sum, sum_old;
	double speed_x, speed_y, speed_z;
	int counter;
};

//new datatype for passing propagation data :P
struct bufferdata
{
	int 	u;
	int 	v;
	int 	x;
 	int 	y;
	int 	z;
};

struct enviroment
{
	double dense;
	double accel;
	double relax;
};

typedef struct 
{
	double mv_x;	
	double mv_y;	
	double mv_z;
	double density;
    bool   solid;
}simProbe;

struct simCoord
{
	int x,
	    y,
	    z;
};

struct simField
{
	int dim_x,
		dim_y,
		dim_z;
};

typedef struct
{
	double min_density;
	double max_density;
	double min_v;
	double max_v;
} minmax_t;

typedef struct 
{
 	int bufferSize;
	int size;
	simProbe* probes;
}receiveBufferInfo;

typedef struct 
{
 	int bufferSize;
	int size;
	bufferdata* cells;
} sendBufferInfo;

typedef vector<receiveBufferInfo*> receiveBuffer_t; 
typedef vector<sendBufferInfo*> sendBuffer_t; 

/*
 * temp structs till integration
 */
typedef struct
{
	float x,y,z;
}coord;

typedef sample_type s_type;

//sim request structs
typedef struct
{
	unsigned int id;
	s_type type;
	
	coord *corners;
}planar;

typedef enum
{
  INTERTYPE_NONE = 0,
  INTERTYPE_DENSITY,
  INTERTYPE_MV_X,
  INTERTYPE_MV_Y,
  INTERTYPE_MV_Z,
} interpolation_type;

#endif
