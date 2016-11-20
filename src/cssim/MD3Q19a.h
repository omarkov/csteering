#include <vector>
#include <mpi.h>
#include "types.h"

class MD3Q19a
{

public:
	MD3Q19a();
	
	virtual ~MD3Q19a();
	
	void collision();
	void propagate();

	double getPressure(int x, int y, int z);
	
	void listenForCommand();
	
private:
	unsigned int max_x;
	unsigned int max_y;
	unsigned int max_z;

	cellData *cellDataModel;
	cellData *bufferLeft;
	cellData *bufferRight;
	
	
	
	double stdRelaxationValue;
	double tau_inv;
	double stdDensity, stdAcceleration;
	double t1_accel, t2_accel;
	
	//double stdAcceleration;
	double t0;
	double t1;
	double t2;

	bool running;

	//MPI Variables
	int myrank;
	int tag, nprocs;
	MPI::Status status;
	bufferdata data;
	
	int leftNB, rightNB;

	cellData* getCell(int x, int y, int z) { 
		return &cellDataModel[(max_x * max_y * z) + (max_x * y) + x]; };
		
	void sendToLeftBuff (int y, int z, int field, double value);
	void sendToRightBuff (int y, int z, int field, double value);
	void waitForArea();
	
	void accelerate();
	void allocateBuffers();
			
	cellData* getBCell(int buffer, int y, int z) { 
		if (buffer == 0) 
			return &bufferLeft[((max_y + 2) * (z + 1)) + (y + 1)];
		if (buffer == 1)
			return &bufferRight[((max_y + 2) * (z + 1)) + (y + 1)];

		return NULL;
		 };
	
		
	// discrete velocity matrix
	int e_x[19];
    int e_y[19];
	int e_z[19];
};
