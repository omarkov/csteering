#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <vector>
#include <iostream>

#include "types.h"
#include "pthread.h"

class BufferManager
{
	public:
		BufferManager();		
		virtual	~BufferManager();

		void setAccel(double accel);
		void setDensity(double density) {stdDensity = density;};
		void setDims(int y, int z) {dim_y = y; dim_z = z;};
		void setNrBuffers(int b) {nBuffers = b;};
		void setRunning(bool b) {running = b;};
				
		void allocateBuffers()
			{	if (buffers) delete[] buffers;
				buffers = new cellData[nBuffers * dim_y * dim_z];};
			
		void propagateBuffers();
		void accelerateField();
		void *run();
		void startBMListener();
		void stopBMListener();
		
		//static void* startManagerThread(void* bmng);
		pthread_t bmngThread;
		
	private:
		//acceleration values
		double t1_accel;
		double t2_accel;
		int nBuffers, dim_y, dim_z;
		double stdDensity;
		
		
		cellData *buffers;
		
		bool running;
		
		MPI::Status status;
		bufferdata incData;
		bufferdata outData;
		
		int buffersize;
		
		
		//returns a pointer to the cell in the buffer asked for
		cellData* getCell(int curBuf, int y, int z) { 
			return &buffers[(nBuffers * dim_y * z) + (nBuffers * y) + curBuf]; };
			
		
		void sendToRight(int curBuf, int y, int z, int field, double value);
		void sendToLeft(int curBuf, int y, int z, int field, double value);
				
		
};

#endif
