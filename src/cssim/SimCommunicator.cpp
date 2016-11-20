#include <vector>
#include <mpi.h>
#include <iostream>

using namespace std;

#include "types.h"
#include "mpitags.h"
#include "values.h"

#include "SimCommunicator.h"
#include "FANClasses.h"

#define INITSIZE 10000

#define EPS  0.00001
#define EPS2 0.00001

extern FAN_Com *simMasterCom;

SimCommunicator::SimCommunicator()
{
    m_running = false;

    //default wert fuer den simulationsraummultiplikator
    factor_x = 3.4;
    factor_y = 1.7;
    factor_z = 1.7;

    stdUpdateRate = 10;

    simulating = false;

    cout << "Simulation Overmind successfully started.." << endl;
}

void SimCommunicator::filterDone()
{
    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Filter_Done);
    }

    MPI::COMM_WORLD.Barrier();
}

void SimCommunicator::filterInit()
{
    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Filter);  
    }
}


SimCommunicator::~SimCommunicator()
{
}

volume_sample *SimCommunicator::computeVolumeSample(sample_save_type * sample_desc)
{
    return NULL;
}

point_sample *SimCommunicator::computePointSample(sample_save_type * sample_desc)
{
    point_sample *probe = new point_sample;
    probe->id = sample_desc->id;
    probe->type = DATATYPE_VECTOR;

    return probe;
}

planar_sample *SimCommunicator::computePlanarSample(sample_save_type * sample_desc)
{
    planar_sample *probe = new planar_sample;

#define RES 256

    probe->id = sample_desc->id;
    probe->type = DATATYPE_SCALAR;
    probe->values = new double[RES * RES];
    probe->u_size = RES;
    probe->v_size = RES;
    probe->dimensionality = 1;

    float planarwidth_x = sample_desc->points[1].x - sample_desc->points[0].x;
    float planarwidth_y = sample_desc->points[1].y - sample_desc->points[0].y;
    float planarwidth_z = sample_desc->points[1].z - sample_desc->points[0].z;

    float planarheight_x = sample_desc->points[3].x - sample_desc->points[0].x;
    float planarheight_y = sample_desc->points[3].y - sample_desc->points[0].y;
    float planarheight_z = sample_desc->points[3].z - sample_desc->points[0].z;

    float x, y, z;
    float start_x = sample_desc->points[0].x + (float) x_sub;
    float start_y = sample_desc->points[0].y + (float) y_sub;
    float start_z = sample_desc->points[0].z + (float) z_sub;

    initSendBuffer();

    for (int v = 0; v < RES; v++) {
        for (int u = 0; u < RES; u++) {
            x = start_x + (planarwidth_x * (float) u / (float) RES) + (planarheight_x * (float) v / (float) RES);
            y = start_y + (planarwidth_y * (float) u / (float) RES) + (planarheight_y * (float) v / (float) RES);
            z = start_z + (planarwidth_z * (float) u / (float) RES) + (planarheight_z * (float) v / (float) RES);

            pushBackCell(u, v, (int) floor(x), (int) floor(y), (int) floor(z));

            probe->values[(RES * v) + u] = 0.0;
        }
    }

    getCells();

    switch (sample_desc->type) {
    case SAMPLETYPE_DENSITY:
        probe->max = minmax.max_density;
        probe->min = minmax.min_density;
        break;
    case SAMPLETYPE_VELOCITY:
        probe->max = sqrt(minmax.max_v);
        probe->min = sqrt(minmax.min_v);
        break;
    case SAMPLETYPE_PRESSURE:
        probe->max = minmax.max_density / 3.0;
        probe->min = minmax.min_density / 3.0;
        break;
    default:
        probe->max = minmax.max_density;
        probe->min = minmax.min_density;
    }


    double currentValue = 0.0;

    for (unsigned i = 0; i < sendBuffer.size(); i++) {
        bufferdata *cells = sendBuffer[i]->cells;
        simProbe *probes = receiveBuffer[i]->probes;

        for (int j = 0; j < sendBuffer[i]->size; j++) {
            bufferdata *cell = &cells[j];
            simProbe *new_probe = &probes[j];
            switch (sample_desc->type) {
            case SAMPLETYPE_DENSITY:
                currentValue = new_probe->density;
                break;
            case SAMPLETYPE_VELOCITY:
                currentValue = sqrt(new_probe->mv_x * new_probe->mv_x + new_probe->mv_y * new_probe->mv_y + new_probe->mv_z * new_probe->mv_z);
                break;
            case SAMPLETYPE_PRESSURE:
                currentValue = new_probe->density / 3.0;
                break;
            default:
                currentValue = new_probe->density;
            }

            if (new_probe->solid) {
                probe->values[(RES * cell->v) + cell->u] = MINFLOAT;
            } else {
                probe->values[(RES * cell->v) + cell->u] = currentValue;
            }
        }
    }
    return probe;
}

glyph_probe_data *SimCommunicator::computeGlyphSample(sample_save_type * sample_desc)
{
    glyph_probe_data *probe = new glyph_probe_data;

    probe->id = sample_desc->id;
    probe->num_streams = sample_desc->count;
    //T B C
    //
    return NULL;
}

void SimCommunicator::getDerivation(const vertex_t & v1, const vertex_t & v2, vertex_t & v, const double &h)
{
    v.x = (v1.x * h + v2.x * h) / (2.0 * h);
    v.y = (v1.y * h + v2.y * h) / (2.0 * h);
    v.z = (v1.z * h + v2.z * h) / (2.0 * h);
}

void SimCommunicator::getRotation(const int &x, const int &y, const int &z, const double &h, simProbe & cell, vertex_t & rot)
{
    vertex_t x1, x2, y1, y2, z1, z2;

    getV(x - 1, y, z, cell, x1);
    getV(x + 1, y, z, cell, x2);
    getDerivation(x1, x2, x1, h);

    getV(x, y - 1, z, cell, y1);
    getV(x, y + 1, z, cell, y2);
    getDerivation(y1, y2, y1, h);

    getV(x, y, z - 1, cell, z1);
    getV(x, y, z + 1, cell, z2);
    getDerivation(z1, z2, z1, h);

    rot.x = z1.y - y1.z;
    rot.y = x1.z - z1.x;
    rot.z = y1.x - x1.y;
}

void SimCommunicator::getVectorProduct(const vertex_t & v1, const vertex_t & v2, vertex_t & v)
{
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
}

double SimCommunicator::getAbs(const vertex_t & v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void SimCommunicator::integrateEuler(const vertex_t & pos, const vertex_t & dir, vertex_t & out, double stepsize)
{
    out.x = pos.x + stepsize * dir.x;
    out.y = pos.y + stepsize * dir.y;
    out.z = pos.z + stepsize * dir.z;
}

void SimCommunicator::getV(const int &x, const int &y, const int &z, simProbe & cell, vertex_t & v)
{
    getCell(x, y, z, cell);
    v.x = cell.mv_x;
    v.y = cell.mv_y;
    v.z = cell.mv_z;
}

ribbon_probe_data *SimCommunicator::computeRibbonSample(sample_save_type * sample_desc, double voxelSize)
{
    ribbon_probe_data *probe = new ribbon_probe_data;
    probe->id = sample_desc->id;
    probe->num_ribbons = sample_desc->count;

    probe->num_vertices = new unsigned int[probe->num_ribbons];

    vertex_t *voxelStartPoints = sample_desc->points;
    vertex_t *startPoints = sample_desc->orig_points;
    simProbe cell;

    probe->min_value = MAXFLOAT;
    probe->max_value = MINFLOAT;
    probe->num_total = 0;

    vector < vertex_t > vertices;
    vector < double >values;

    vertices.reserve(probe->num_ribbons * dim_x * 2 * 2);
    values.reserve(probe->num_ribbons * dim_x * 2);

    for (int i = 0; i < (int) probe->num_ribbons; i++) {
        int start_x = (int) voxelStartPoints[i].x + x_sub;
        int start_y = (int) voxelStartPoints[i].y + y_sub;
        int start_z = (int) voxelStartPoints[i].z + z_sub;

        double sx = startPoints[i].x;
        double sy = startPoints[i].y;
        double sz = startPoints[i].z;

        int size = 0;

        vertex_t pos;
        vertex_t pos_dist;
        vertex_t last_pos;
        vertex_t last_v;
        vertex_t rot;

        vertex_t v;
        vertex_t v2;

        pos.x = sx;
        pos.y = sy;
        pos.z = sz;

        int last_x = -1;
        int last_y = -1;
        int last_z = -1;

        double val = 0.0;

        cell.solid = false;

        while (last_x < dim_x && steps > 200 && size < dim_x * 2) {
            int x = start_x + (int) ((pos.x - sx) / voxelSize);
            int y = start_y + (int) ((pos.y - sy) / voxelSize);
            int z = start_z + (int) ((pos.z - sz) / voxelSize);

            if (x != last_x || y != last_y || z != last_z) {
                getV(x, y, z, cell, v);
                val = getAbs(v);

                if (val < EPS2) {
                    //                   cout << "Cut ribbon: ABS=" << val << endl;
                    break;
                }

                if (val < EPS) {
                    v.x = v.x / val * EPS;
                    v.x = v.y / val * EPS;
                    v.x = v.z / val * EPS;
                    val = EPS;
                }

                vertices.push_back(pos);
                pos_dist = pos;

                if (val < probe->min_value)
                    probe->min_value = val;
                if (val > probe->max_value)
                    probe->max_value = val;

                getRotation(x, y, z, voxelSize, cell, rot);
                getVectorProduct(v, rot, v2);

                pos_dist.x += rot.x * sample_desc->height / val;
                pos_dist.y += rot.y * sample_desc->height / val;
                pos_dist.z += rot.z * sample_desc->height / val;

                vertices.push_back(pos_dist);

                values.push_back(val);

                last_x = x;
                last_y = y;
                last_z = z;

                last_pos = pos;
                last_v = v;

                size++;
                probe->num_total++;
            }

            integrateEuler(pos, v, pos, 1.0);
        }

        probe->num_vertices[(unsigned int) i] = size * 2;
    }

    probe->values = new double[values.size()];
    probe->ribbons = new vertex_t[vertices.size()];

    for (unsigned int i = 0; i < values.size(); i++)
        probe->values[i] = values[i];

    for (unsigned int i = 0; i < vertices.size(); i++)
        probe->ribbons[i] = vertices[i];

    return probe;
}

void SimCommunicator::simStepOn()
{

    m_running = true;

    FAN_postMessage(simMasterCom, "started", NULL, NULL);

    while (simulating) {

//        cout << "simStepOn ..." << endl;
//      cout << "Overmind waiting for synch.." << endl;
//      MPI::COMM_WORLD.Barrier();
//      cout << "\t OM " << " synched!";

        for (int sim = 1; sim < nprocs; sim++) {
//          cout << "Trying to trigger Sim " << sim << "." << endl;
            //propagate
//          MPI::COMM_WORLD.Sendrecv_replace(NULL, 0, MPI::BYTE, sim, MPI_Model_Propagate, sim, MPI_Ack, status);
            MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Model_Propagate);
/*          if ((status.Get_tag() == MPI_Ack) && (status.Get_source() == sim ))
            {
//              cout << "PROPAGATION triggered on Sim " << sim << "; Received Ack.." << endl;
            }           */
        }
        MPI::COMM_WORLD.Barrier();

//      cout << "Waiting for propagations to finish.." << endl;
        MPI::COMM_WORLD.Barrier();

//      cout << "Waiting for even Sims to propagate their Buffers" << endl;
        MPI::COMM_WORLD.Barrier();

//      cout << "Waiting for odd Sims to propagate their Buffers" << endl;
        MPI::COMM_WORLD.Barrier();

        //collision     
        for (int sim = 1; sim < nprocs; sim++) {
            //collision
//          MPI::COMM_WORLD.Sendrecv_replace(NULL, 0, MPI::BYTE, sim, MPI_Model_Collision, sim, MPI_Ack, status);
            MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Model_Collision); // , sim, MPI_Ack, status);
/*          if ((status.Get_tag() == MPI_Ack) && (status.Get_source() == sim ))
            {
//              cout << "COLLISION triggered on Sim" << sim << "; Received Ack.." << endl;
            }       */
        }

//      cout << "Waiting for collision handling" << endl;
        MPI::COMM_WORLD.Barrier();

        filterInit();
        if (!(steps % stdUpdateRate) && simMasterCom != NULL)
            FAN_sendMessage(simMasterCom, "filter", (void *) steps);

        filterDone();

        steps++;

    }
    //MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Sim_Halted);
    if (simMasterCom != NULL) {
        FAN_postMessage(simMasterCom, "paused", NULL, NULL);
    }
    m_running = false;
    return;
}

void SimCommunicator::pauseSim()
{
    simulating = false;
}

void SimCommunicator::bPauseSim()
{
    simulating = false;

    while (m_running);
}

void SimCommunicator::resetMinMax()
{
    minmax.min_density = MAXFLOAT;
    minmax.max_density = MINFLOAT;
    minmax.min_v = MAXFLOAT;
    minmax.max_v = MINFLOAT;
}

void SimCommunicator::startSim()
{
//  cout << "Starting SimSteps" << endl;
    resetMinMax();
    simulating = true;
    simStepOn();
}

void SimCommunicator::updateVars()
{
    myrank = MPI::COMM_WORLD.Get_rank();
    nprocs = MPI::COMM_WORLD.Get_size();

    if (receiveBuffer.size() < (unsigned int) nprocs - 1) {
        for (int i = 0; i < nprocs - 1; i++) {
            receiveBufferInfo *rbi = new receiveBufferInfo;
            rbi->bufferSize = INITSIZE;
            rbi->size = 0;
            rbi->probes = new simProbe[INITSIZE];
            receiveBuffer.push_back(rbi);
            sendBufferInfo *sbi = new sendBufferInfo;
            sbi->bufferSize = INITSIZE;
            sbi->size = 0;
            sbi->cells = new bufferdata[INITSIZE];
            sendBuffer.push_back(sbi);
        }
    }

    MPI::Status s;

    enviroment updated;
    updated.accel = stdAcceleration;
    updated.dense = stdDensity;
    updated.relax = stdRelaxation;

    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Sendrecv_replace(NULL, 0, MPI::BYTE, sim, MPI_Update_Enviroment, sim, MPI_Ack, s);
        if ((s.Get_tag() == MPI_Ack) && (s.Get_source() == sim)) {
            MPI::COMM_WORLD.Sendrecv_replace(&updated, sizeof(enviroment), MPI::BYTE, sim, MPI_Update_Enviroment, sim, MPI_Ack, s);
            if ((s.Get_tag() == MPI_Ack) && (s.Get_source() == sim)) {
                //          cout << "Enviroment update successful on Sim" << sim << endl;
            }
        }
    }

    MPI::COMM_WORLD.Barrier();
}

void SimCommunicator::setFactor(double f_x, double f_y, double f_z)
{
    setVoxels(voxels, f_x, f_y, f_z);
}

void SimCommunicator::setFactor(double f)
{
    setVoxels(voxels, f);
}

void SimCommunicator::setVoxels(const Voxels & v)
{
    setVoxels(v, factor_x, factor_y, factor_z);
}

void SimCommunicator::updateVoxels(const Voxels & v)
{
    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Sendrecv(NULL, 0, MPI::BYTE, sim, MPI_Update_Area, NULL, 0, MPI::BYTE, MPI_ANY_SOURCE, MPI_Ack, status);
    }

    bufferdata out;

    for (int z = z_sub; z < (dim_z - z_sub); z++) {
        for (int y = y_sub; y < (dim_y - y_sub); y++) {
            for (int x = x_sub; x < (dim_x - x_sub); x++) {
                if (v[x - x_sub][y - y_sub][z - z_sub] != voxels[x - x_sub][y - y_sub][z - z_sub]) {
                    out.x = x % sliceWidth;
                    out.y = y;
                    out.z = z;

                    MPI::COMM_WORLD.Send(&out, sizeof(bufferdata), MPI::BYTE, x / sliceWidth + 1, MPI_Update_Field);
                }
            }
        }
    }

    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Update_Field_Done);
    }

    MPI::COMM_WORLD.Barrier();

    voxels = v;
}

void SimCommunicator::getDimensions(int &x, int &y, int &z)
{
    x = dim_x;
    y = dim_y;
    z = dim_z;
}

void SimCommunicator::setVoxels(const Voxels & v, double f)
{
    setVoxels(v, f, f, f);
}

void SimCommunicator::setVoxels(const Voxels & v, double f_x, double f_y, double f_z)
{
    steps = 0;

    voxels = v;
    factor_x = f_x;
    factor_y = f_y;
    factor_z = f_z;

    int old_dim_x = v.size();
    int old_dim_y = v[0].size();
    int old_dim_z = v[0][0].size();

    dim_x = (int) ((double) old_dim_x * factor_x);
    dim_y = (int) ((double) old_dim_y * factor_y);
    dim_z = (int) ((double) old_dim_z * factor_z);

//        cout << "dim: " << dim_x << " procs: " << nprocs << endl;
    sliceLastWidth = (int) (dim_x % (nprocs - 1));
    sliceWidth = (int) (dim_x / (nprocs - 1));

    if (sliceLastWidth != 0) {
        sliceWidth = (int) (dim_x / (nprocs - 2));
        sliceLastWidth = (int) (dim_x % (nprocs - 2));
    } else {
        sliceLastWidth = sliceWidth;
    }

    if (sliceLastWidth == 0) {
        sliceLastWidth += 3;
        dim_x += 3;
    }


    if ((dim_x - old_dim_x) % 2)
        dim_x++;
    if ((dim_y - old_dim_y) % 2)
        dim_y++;
    if ((dim_z - old_dim_z) % 2)
        dim_z++;

    x_sub = (dim_x - old_dim_x) / 2;
    y_sub = (dim_y - old_dim_y) / 2;
    z_sub = (dim_z - old_dim_z) / 2;

    cout << "DIM: " << dim_x << "/" << dim_y << "/" << dim_z << endl;


    int last = nprocs - 1;
    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Sendrecv(NULL, 0, MPI::BYTE, sim, MPI_Set_Area, NULL, 0, MPI::BYTE, MPI_ANY_SOURCE, MPI_Ack, status);
        simField data;

        data.dim_x = sliceWidth;
        data.dim_y = dim_y;
        data.dim_z = dim_z;

        if (sim == last) {
            data.dim_x = sliceLastWidth;
        }

        MPI::COMM_WORLD.Send(&data, sizeof(simField), MPI::BYTE, sim, (MPI_Set_Area + 1));
    }

    bufferdata out;

    for (int z = z_sub; z < (dim_z - z_sub); z++) {
        for (int y = y_sub; y < (dim_y - y_sub); y++) {
            for (int x = x_sub; x < (dim_x - x_sub); x++) {
                if (voxels[x - x_sub][y - y_sub][z - z_sub]) {
                    out.x = x % sliceWidth;
                    out.y = y;
                    out.z = z;

                    MPI::COMM_WORLD.Send(&out, sizeof(bufferdata), MPI::BYTE, x / sliceWidth + 1, MPI_Field);
                }
            }
        }
    }

    for (int sim = 1; sim < nprocs; sim++) {
        MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, sim, MPI_Field_Done);
    }

//  cout << "Overmind waiting for synch.." << endl;
    MPI::COMM_WORLD.Barrier();
//  cout << "\t OM " << " synched!";
}

void SimCommunicator::haltSim()
{
    bPauseSim();
//  cout << "Broadcasting Disconnect to all Processes..";
    for (int c = 1; c < nprocs; c++) {
        MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, c, MPI_Disconnect);
    }
}

void SimCommunicator::initSendBuffer()
{
    for (int i = 0; i < nprocs - 1; i++) {
        sendBuffer[i]->size = 0;
    }
}

void SimCommunicator::pushBackCell(int u, int v, int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || x >= dim_x || y >= dim_y || z >= dim_z) {
        return;
    }

    int targetSim = ((int) (x / sliceWidth)) + 1;

    if (targetSim > nprocs - 1)
        targetSim = nprocs - 1;

    targetSim--;

    int size = sendBuffer[targetSim]->size;
    int bufferSize = sendBuffer[targetSim]->bufferSize;

    if (size == bufferSize) {
        bufferdata *buffer = new bufferdata[bufferSize * 2];
        memcpy(buffer, sendBuffer[targetSim]->cells, sizeof(bufferdata) * size);
        sendBuffer[targetSim]->bufferSize *= 2;

        delete[]sendBuffer[targetSim]->cells;
        sendBuffer[targetSim]->cells = buffer;
    }

    sendBuffer[targetSim]->cells[size].u = u;
    sendBuffer[targetSim]->cells[size].v = v;
    sendBuffer[targetSim]->cells[size].x = x % sliceWidth;
    sendBuffer[targetSim]->cells[size].y = y;
    sendBuffer[targetSim]->cells[size].z = z;
    sendBuffer[targetSim]->size++;
}

void SimCommunicator::getCell(int x, int y, int z, simProbe & probe)
{
    if (x < 0 || y < 0 || z < 0 || x >= dim_x || y >= dim_y || z >= dim_z) {
        probe.mv_x = 0;
        probe.mv_y = 0;
        probe.mv_z = 0;
        probe.density = 0;
        probe.solid = true;


        return;
    }

    int targetSim = ((int) (x / sliceWidth)) + 1;

    if (targetSim > nprocs - 1)
        targetSim = nprocs - 1;

    simCoord coord;
    coord.x = x % sliceWidth;
    coord.y = y;
    coord.z = z;

    MPI::COMM_WORLD.Send(&coord, sizeof(simCoord), MPI::BYTE, (int) targetSim, MPI_Get_Cell);
    MPI::COMM_WORLD.Recv(&probe, sizeof(simProbe), MPI::BYTE, (int) targetSim, MPI_Get_Cell, status);
}


void SimCommunicator::getCells()
{
    unsigned int count = 0;
    unsigned int i;

    for (i = 0; i < sendBuffer.size(); i++) {
        bufferdata *buffer = sendBuffer[i]->cells;
        int size = sendBuffer[i]->size;

        if (size > 0) {
            count++;
            MPI::COMM_WORLD.Send(&size, sizeof(int), MPI::BYTE, (int) i + 1, MPI_Set_Req_Size);
            MPI::COMM_WORLD.Send(buffer, sizeof(bufferdata) * size, MPI::BYTE, (int) i + 1, MPI_Get_Cells);
        }
    }

    minmax_t m_minmax;
    int rsize;

    for (unsigned int n = 0; n < count * 3; n++) {
        MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, status);
        i = status.Get_source() - 1;

        switch (status.Get_tag()) {
        case MPI_Send_MinMax:
            {
                MPI::COMM_WORLD.Recv(&m_minmax, sizeof(minmax_t), MPI::BYTE, i + 1, MPI_Send_MinMax, status);
                if (m_minmax.min_density < minmax.min_density)
                    minmax.min_density = m_minmax.min_density;
                if (m_minmax.max_density > minmax.max_density)
                    minmax.max_density = m_minmax.max_density;
                if (m_minmax.min_v < minmax.min_v)
                    minmax.min_v = m_minmax.min_v;
                if (m_minmax.max_v > minmax.max_v)
                    minmax.max_v = m_minmax.max_v;
                break;
            }
        case MPI_Set_Resp_Size:
            {
                MPI::COMM_WORLD.Recv(&rsize, sizeof(int), MPI::BYTE, i + 1, MPI_Set_Resp_Size, status);
                if (rsize > receiveBuffer[i]->bufferSize) {
                    delete[]receiveBuffer[i]->probes;
                    receiveBuffer[i]->probes = new simProbe[rsize];
                    receiveBuffer[i]->bufferSize = rsize;
                }
                receiveBuffer[i]->size = rsize;
                break;
            }
        case MPI_Get_Cells:
            {
                MPI::COMM_WORLD.Recv(receiveBuffer[i]->probes, sizeof(simProbe) * receiveBuffer[i]->size, MPI::BYTE, i + 1, MPI_Get_Cells, status);
                break;
            }
        }
    }
}
