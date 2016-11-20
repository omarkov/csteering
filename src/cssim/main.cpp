#include <stdlib.h>
#include <vector>
#include <iostream>
#include "mpi.h"
using namespace std;

#include "MD3Q19a.h"
#include "types.h"
#include "mpitags.h"
#include "SimCommunicator.h"

//#define NUM_THREADS 2

SimCommunicator simulation;

int main ()
{
	//Init MPI
//	MPI::Init();
	int myrank = MPI::COMM_WORLD.Get_rank();
	int nprocs = MPI::COMM_WORLD.Get_size();
	
	Voxels testVoxelArray;
	
	if (myrank == 0)
	{
		testVoxelArray.resize(50);
			
		for (unsigned int x = 0; x < 50; x++) {
			testVoxelArray[x].resize(50);
			for (unsigned int y = 0; y < 50; y++) {
				testVoxelArray[x][y].resize(50);
				for (unsigned int z = 0; z < 50; z++) {
					if (y == 0) { testVoxelArray[x][y][z] = 1;	continue;}
					if (y == 50) { testVoxelArray[x][y][z] = 1;	continue;}
					if (z == 50) { testVoxelArray[x][y][z] = 1;	continue;}
					if (z == 0) { testVoxelArray[x][y][z] = 1;	continue;}
					if (x > 20) 
					{
						if (y > 20) 
						{ 
							if (z > 20) 
							{ 
								if (x < 30) 
								{ 
									if (y < 25) 
									{ 
										if (z < 25) 
										{
										  testVoxelArray[x][y][z] = 1;
										}
									}
								}
							}
						}
					} else {
						testVoxelArray[x][y][z] = 0;
					}
				}
			}
		}
	}
	
	

	if (myrank == 0)
	{
		// Init parameters for pressure=rho, acceleration and density=omega
		// all parameters must be changed for dynamic GUI-Input!!!
		
		simulation.setAccel(0.001);
		simulation.setDensity(0.01);
		simulation.setRelax(0.002);
//		cout << "Envi Param set." << endl;
		simulation.updateVars();
		
//		MPI::COMM_WORLD.Barrier();
//		cout << "all synched" << endl;
		simulation.setVoxels(testVoxelArray, 6);
//		cout << "Voxel Set" << endl;
		
		simulation.startSim();	
//		cout << "Sim Started" << endl;
//		sleep(10);
//		simulation.haltSim();
	}
	
	return 0;	
}
