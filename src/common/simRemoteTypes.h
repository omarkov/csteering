#ifndef SIM_REMOTE_TYPES_H
#define SIM_REMOTE_TYPES_H

#include "RemoteInterface.h"


// ----------------------------------------------------------------------------
// Communication with the simulation
// ----------------------------------------------------------------------------

// general definitions

typedef enum
{
  SAMPLETYPE_NONE = 0,
  SAMPLETYPE_PRESSURE,
  SAMPLETYPE_VELOCITY,
  SAMPLETYPE_DENSITY
} sample_type;

typedef enum
{
  PROBETYPE_POINT=0,
  PROBETYPE_PLANE,
  PROBETYPE_VOLUME,
  PROBETYPE_RIBBON,
  PROBETYPE_GLYPH
} probe_type;

typedef enum
{
  DATATYPE_SCALAR=0,
  DATATYPE_VECTOR,
  DATATYPE_TENSOR
} data_type;


// generic sample types

typedef struct
{
  int x;
  int y;
  int z;
} coord_type;

typedef struct
{
  unsigned int id;

  data_type type;
  unsigned int dimensionality;
  double min, max;

  double *value;
} point_sample;


typedef struct
{
  unsigned int id;

  data_type type;
  unsigned int dimensionality;
  double min, max;
  unsigned int u_size, v_size;

  double *values;
} planar_sample;


typedef struct
{
  unsigned int id;

  data_type type;
  unsigned int dimensionality;
  double min, max;
  unsigned int u_size, v_size, w_size;

  double *values;
} volume_sample;

typedef struct
{
    int id;
    sample_type type;
    probe_type ptype;
    double   height;
    int      count;
    vertex_t *points;
} sample_desc_type;

typedef struct
{
    int id;
    sample_type type;
    probe_type ptype;
    double  height;
    int      count;
    vertex_t  *points;
    vertex_t  *orig_points;
} sample_save_type;

// special probe samples

typedef struct
{
  unsigned int id;

  unsigned int num_ribbons;
  unsigned int *num_vertices;
  unsigned int num_total;

  vertex_t *ribbons; // num_ribbons * num_segments * 2
  double *values; // num_ribbons * num_segments

  double min_value, max_value;
} ribbon_probe_data;

typedef struct
{
  unsigned int id;

  unsigned int num_streams;
  unsigned int *num_glyphs_per_stream;
  unsigned int num_total;

  double min_value, max_value;

  vertex_t *positions; // num_streams * num_glyphs_per_stream
  vertex_t *directions; // num_streams * num_glyphs_per_stream
  double *values; // num_streams * num_glyphs_per_stream
} glyph_probe_data;


#endif // SIM_REMOTE_TYPES_H
