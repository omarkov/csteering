#ifndef SIMCOMMUNICATOR_H
#define SIMCOMMUNICATOR_H

#include <vector>
#include <mpi.h>
#include "types.h"
#include "../common/simRemoteTypes.h"

class SimCommunicator
{
    public:
        SimCommunicator();
        
        virtual ~SimCommunicator();
    
        void setVoxels(const Voxels& v);
        void updateVoxels(const Voxels& v);
        void setVoxels(const Voxels& voxels, double f);
        void setVoxels(const Voxels& voxels, double f_x, double f_y, double f_z);
        void setFactor(double f);
        void setFactor(double f_x, double f_y, double f_z);
        void updateVars();
        
        //public Setter
        void setAccel   (double accel)      {stdAcceleration = accel;};
        void setDensity (double density)    {stdDensity = density;};
        void setRelax   (double relax)      {stdRelaxation = relax;};
        void setUpdateRate   (int rate)      {stdUpdateRate = rate;};

        double getAbs(const vertex_t &rot);

        void getDerivation(const vertex_t &v1, const vertex_t &v2, vertex_t &v, const double &h);
        void getRotation(const int &x, const int &y, const int &z, const double &h, simProbe &cell, vertex_t &rot);
        void getVectorProduct(const vertex_t &v1, const vertex_t &v2, vertex_t &v);
        void getV(const int &x, const int &y, const int &z, simProbe &cell, vertex_t &v);

        point_sample      *computePointSample(sample_save_type *sample_desc);
        planar_sample     *computePlanarSample(sample_save_type *sample_desc);
        volume_sample     *computeVolumeSample(sample_save_type *sample_desc);
        glyph_probe_data  *computeGlyphSample(sample_save_type *sample_desc);
        ribbon_probe_data *computeRibbonSample(sample_save_type *sample_desc, double voxelSize);
        
        //Sim-Controllers
        void bPauseSim  ();
        void pauseSim   ();
        void startSim   ();
        void haltSim    ();
        void resetMinMax();
        void filterDone();
        void filterInit();

        void getDimensions(int &x, int &y, int &z);

        void simStepOn();
        
    private:
        void sendSlice(int sim, int min_x, int max_x);
        
        bool simulating;
        
        Voxels voxels;

        //MPI variables
        MPI::Status status;
    
        int steps;
        int myrank, nprocs;
        int dim_x, dim_y, dim_z;
        int z_sub, y_sub, x_sub;
        int sliceWidth, sliceLastWidth;
        double factor_x, factor_y, factor_z;
        bool m_running;

        receiveBuffer_t receiveBuffer;
        sendBuffer_t sendBuffer;
        minmax_t minmax;
        
        double stdAcceleration, stdDensity, stdRelaxation;
        int stdUpdateRate;
        
        void initSendBuffer();
        void pushBackCell(int u, int v, int x, int y, int z);
        void getCell(int x, int y, int z, simProbe &probe);
        void getCells();

        void integrateEuler(const vertex_t &pos, const vertex_t &dir, vertex_t &out, double stepsize);
};

#endif
