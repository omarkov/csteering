#ifndef _MD3Q19_NOMPI_H_
#define _MD3Q19_NOMPI_H_

#include <vector>
#include "types.h"

class MD3Q19a
{
public:
	MD3Q19a();
	
	virtual ~MD3Q19a();
	
	//testvalue for a middensity
	middens overall;
	
	void collision();
	void propagate();
	void redistribute();
	void initModel(const Voxels& voxels, double stdRelaxationValue, double stdDensity, double stdAcceleration);
	void initCell();
	double getPressure(int x, int y, int z);
	
private:
	unsigned int max_x;
	unsigned int max_y;
	unsigned int max_z;
	cellData *cellDataModel;
	double stdRelaxationValue;
	double tau_inv;
	double stdDensity;
	double stdAcceleration;
	double t0;
	double t1;
	double t2;

	cellData *pbcbuffer;


	//MPI Variables
	int myrank;
	int overmind, worker, tag, nprocs;
	


	cellData* getCell(int x, int y, int z) { 
//		return &cellDataModel[((max_x + 2)*(max_y + 2)*(z + 1)) + ((max_x + 2)*(y + 1)) + (x + 1)]; };
		return &cellDataModel[(max_x * max_y * z) + (max_x * y) + x]; };

	cellData* getBufferCell(int y, int z) { 
		return &pbcbuffer[(max_y * z) + y]; };
		
	void sendToLeftBuff (int y, int z, int field, double value);
	void sendToRightBuff (int y, int z, int field, double value);
		
	// discrete velocity matrix
	int e_x[19];
    int e_y[19];
	int e_z[19];
};

#endif //_MD3Q19_NOMPI_H_
