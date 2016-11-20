#include <vector>
#include <mpi.h>
#include "types.h"

class MD3Q19b
{

public:
	MD3Q19b();
	
	virtual ~MD3Q19b();
	
	void collision();
	void propagate();
	void filter();

	double getPressure(int x, int y, int z);
	
	void listenForCommand();
	
private:
	unsigned int max_x;
	unsigned int max_y;
	unsigned int max_z;

	cellData *cellDataModel;
	bufferData *bufferLeft;
	bufferData *bufferRight;
	bufferData *buffer;

	sendBufferInfo receiveBuffer;
	receiveBufferInfo sendBuffer;
	
	double stdRelaxationValue;
	double tau_inv;
	double stdDensity, stdAcceleration;
	double t1_accel, t2_accel;
	
	//double stdAcceleration;
	double t0;
	double t1;
	double t2;

	minmax_t minmax;

	bool running;

	//MPI Variables
	int myrank;
	int tag, nprocs;
	MPI::Status status;
	bufferdata data;

	cellData *outOfBounds;
	
	int leftNB, rightNB;

	cellData* getCell(int x, int y, int z) { 
		if(x < 0 || y < 0 || z < 0 || x >= (int)max_x || y >= (int)max_y || z >= (int)max_z)
		    	return outOfBounds;
			
		return &cellDataModel[(max_x * max_y * z) + (max_x * y) + x]; 
	};

	bufferData* getCell(bufferData *buffer, int y, int z) { 
		return &buffer[max_y * z + y]; 
	};
		
	void sendToLeftBuff (int y, int z, int field, double value);
	void sendToRightBuff (int y, int z, int field, double value);
	void waitForArea();
	void waitForUpdatedArea();
	
	void accelerateFW();
	void accelerateBW();
	void allocateBuffers();

	void MD3Q19b::initCell(cellData *cell);
	void MD3Q19b::initBCell(bufferData *cell);
			
	bufferData* getBCell(int buffer, int y, int z) { 
		if (buffer == 0) 
			return &bufferLeft[max_y * z + y];
		if (buffer == 1)
			return &bufferRight[max_y * z + y];

		return NULL;
		 };
	
		
	// discrete velocity matrix
	int e_x[19];
    int e_y[19];
	int e_z[19];
};
