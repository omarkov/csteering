#include <vector>
#include <iostream>
#include "mpi.h"

using namespace std; 

#include "mpitags.h"
#include "BufferManager.h"


static void* startManagerThread(void* bmng) {
  ((BufferManager*) bmng)->run();

  return NULL;
}

BufferManager::BufferManager()
{
}

void BufferManager::startBMListener()
{
	//cleaning incData
	incData.y = 0;
	incData.z = 0;
	incData.field = 0;
	incData.value = 0.;
		
	buffersize = (int)(sizeof(incData));
	
	
	int rc = pthread_create(&bmngThread, NULL, startManagerThread, this);
	 if (0 != rc) {
    	fprintf(stderr, "CommandServer::CommandServer() - could not create "
	    "the server thread, %i returned by pthread_create\n", rc);
	 } else { cout << "BufferManager Runthread successfully created" << endl;}
	
}

void BufferManager::stopBMListener()
{
	MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Disconnect);
	pthread_join(bmngThread,NULL);	
}

BufferManager::~BufferManager()
{
}

void BufferManager::propagateBuffers()
{
	//propagate buffers here
	cellData* next;
	cellData* current;
	
	for (int curBuf = 0; curBuf < nBuffers; curBuf++)
	{
		
		// backward
		for (int z = 1; z < dim_z -1; z++) 
		{
			for (int y = 1; y < dim_y -1; y++) 
			{
				current = getCell(curBuf, y, z);
				
				//fields going to left domain
				// 2
				sendToLeft(curBuf, y, z, 2, current->distributionValue[2]);
				// 8
				sendToLeft(curBuf, y - 1, z, 8, current->distributionValue[8]);
				// 12
				sendToLeft(curBuf, y, z - 1, 12, current->distributionValue[12]);
	
	
				//fields going to right domain
				// 9
				sendToRight(curBuf, y - 1, z, 9, current->distributionValue[9]);
				// 13
				sendToRight(curBuf, y, z - 1, 13, current->distributionValue[13]);
	
	
				/*
				 * Wahrscheinlich unnütz da hier auch nicht drübergerechnet wird.
				 * 		
					// 4
					next = getCell(curBuf, y - 1, z);
					next->distributionValue[4] = current->distributionValue[4];
					// 6
					next = getCell(curBuf, y, z - 1);
					next->distributionValue[6] = current->distributionValue[6];
					// 16
					next = getCell(curBuf, y - 1, z - 1);
					next->distributionValue[16] = current->distributionValue[16];
					// 17
					next = getCell(curBuf, y + 1, z - 1);
					next->distributionValue[17] = current->distributionValue[17];
				*/
			}
		}
	
		// forward
		for (int z = dim_z -1; z-- > 1; ) 
		{
			for (int y = dim_y -1; y-- > 1; ) 
			{
				current = getCell(curBuf, y, z);
	
				//fields going to right domain
				sendToRight(curBuf, y, z, 1, current->distributionValue[1]);
				// 7
				sendToRight(curBuf, y + 1, z, 7,current->distributionValue[7]);
				// 11
				sendToRight(curBuf, y, z + 1, 11, current->distributionValue[11]);
				
				//fields going to left domain
				// 10
				sendToLeft(curBuf, y + 1, z, 10, current->distributionValue[10]);
				// 14
				sendToLeft(curBuf, y, z + 1, 14, current->distributionValue[14]);
				
				/*
				 * Wahrscheinlich unnütz da hier auch nicht drübergerechnet wird.
				 * 		
					// 15
					next = getCell(curBuf, y + 1, z + 1);
					next->distributionValue[15] = current->distributionValue[15];
					// 18
					next = getCell(curBuf, y - 1, z + 1);
					next->distributionValue[18] = current->distributionValue[18];
					// 3
					next = getCell(curBuf, y + 1, z);
					next->distributionValue[3] = current->distributionValue[3];
					// 5
					next = getCell(curBuf, y, z + 1);
					next->distributionValue[5] = current->distributionValue[5];
				*/
			}
		}
	}	
}

void BufferManager::accelerateField()
{
	//process acceleration on buffer 0
	
	for (unsigned int y = 1; y < dim_y-1; y++) 
	{
		for (unsigned int z = 1; z < dim_z-1; z++) 
		{
			cellData* c = getCell(0, y, z);
			
			if (c->distributionValue[2] - t1_accel > 0 &&
				c->distributionValue[8] - t2_accel > 0 && 
				c->distributionValue[10] - t2_accel > 0 && 
				c->distributionValue[12] - t2_accel > 0 && 
				c->distributionValue[14] - t2_accel > 0)
			{
				c->distributionValue[1] += (double)  t1_accel;
				c->distributionValue[2] += (double) -t1_accel;
	
				c->distributionValue[7] += (double)  t2_accel;
				c->distributionValue[8] += (double) -t2_accel;
				c->distributionValue[9] += (double)  t2_accel;
				c->distributionValue[10] += (double)-t2_accel;
				c->distributionValue[11] += (double) t2_accel;
				c->distributionValue[12] += (double)-t2_accel;
				c->distributionValue[13] += (double) t2_accel;
				c->distributionValue[14] += (double)-t2_accel;
			}
		}
	}	
}

void BufferManager::sendToRight(int curBuf, int y, int z, int field, double value)
{
	outData.y = y; 
	outData.z = z; 
	outData.field = field; 
	outData.value = value;;

	//send the data:   	                            	dtype      receiver     , tag  
	MPI::COMM_WORLD.Sendrecv(&outData, sizeof(outData), MPI::BYTE, (curBuf + 1) , MPI_Data_IncLeft, NULL, 0, MPI::BYTE, MPI_ANY_SOURCE, MPI_Ack, status);
	if ((status.Get_tag() == MPI_Ack) && (status.Get_source() == (curBuf + 1)) )
	{
		cout << "Successfully sent to " << (curBuf +1) << "; Received ACK" << endl;
	}
}

void BufferManager::sendToLeft(int curBuf, int y, int z, int field, double value)
{
	outData.y = y; 
	outData.z = z; 
	outData.field = field; 
	outData.value = value;;

	//send the data:   	                            dtype             receiver                      , tag  
	MPI::COMM_WORLD.Sendrecv(&outData, sizeof(outData), MPI::BYTE, ((curBuf + nBuffers - 1) % nBuffers) , MPI_Data_IncRight, NULL, 0, MPI_BYTE, MPI_ANY_SOURCE, MPI_Ack, status);
	if ((status.Get_tag() == MPI_Ack) && (status.Get_source() == ((curBuf + nBuffers - 1) % nBuffers)) )
	{
		cout << "Successfully sent to " << ((curBuf + nBuffers - 1) % nBuffers ) << "; Received ACK" << endl;
	}
}


//Keep the eyes open
void *BufferManager::run()
{	
	running = true;
	int targetBuffer;
	
	while(running)
	{
		//listening for incoming data
		MPI::COMM_WORLD.Recv(&incData, buffersize, MPI::BYTE, MPI_ANY_SOURCE, (MPI_Data_OutRight || MPI_Data_OutLeft), status);
		
		switch (status.Get_tag())
		{
			case MPI_Data_OutRight:
			{
				targetBuffer = ((status.Get_source()) % nBuffers);
				getCell(targetBuffer, incData.y, incData.z)->distributionValue[incData.field] = incData.value;
			}
			
			case MPI_Data_OutLeft:
			{
				targetBuffer = ((status.Get_source()) - 1);
				getCell(targetBuffer, incData.y, incData.z)->distributionValue[incData.field] = incData.value;
			}

			case MPI_Disconnect:
			{
				running = false;
			}
			
			default: 
			{
				cout << "Error receiving Message from: " << status.Get_source() << endl;
				cout << "Received unknown Tag: " << status.Get_tag() << endl;
			}
		}
	}
	pthread_exit(NULL);
}
	

void BufferManager::setAccel(double accel)
{
	//setting t1 and t2 for acceleration
	t1_accel = accel * stdDensity / 19.;
	t2_accel = accel * stdDensity / 38.;
}

