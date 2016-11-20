#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

#include "MD3Q19_noMPI.h"
#include "types.h"
//#include "BufferManager.h"

int main ()
{
	// Init parameters for pressure=rho, acceleration and density=omega
	// all parameters must be changed for dynamic GUI-Input!!!
	double stdDensity = 1.85;			//fluiddensity
	double stdAcceleration = 0.03;		//für redistribution
	double stdRelaxationValue = 0.2;	//Relax / viscosity
	//wenn fertig


	//muss wech
	int testArrayDims = 20;
	
	Voxels testVoxelArray;
	testVoxelArray.resize(testArrayDims);
		
	for (unsigned int x = 0; x < testArrayDims; x++) {
		testVoxelArray[x].resize(testArrayDims);
		for (unsigned int y = 0; y < testArrayDims; y++) {
			testVoxelArray[x][y].resize(testArrayDims);
			for (unsigned int z = 0; z < testArrayDims; z++) {
				if (y == 0) { testVoxelArray[x][y][z] = 1; continue;}
				if (y == testArrayDims-1) { testVoxelArray[x][y][z] = 1;continue;}
				if (z == testArrayDims-1) { testVoxelArray[x][y][z] = 1;continue;}
				if (z == 0) { testVoxelArray[x][y][z] = 1;continue;}
				if (x > 10) 
				{
					if (y > 10) 
					{ 
						if (z > 10) 
						{ 
							if (x < 14) 
							{ 
								if (y < 14) 
								{ 
									if (z < 14) 
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
		
	MD3Q19a gridModel;
	gridModel.initModel(testVoxelArray, stdRelaxationValue, stdDensity, stdAcceleration);
		

	//synch MPI
	while (1)
	{
		gridModel.propagate();
		gridModel.collision();		
		gridModel.redistribute();
	}

	//Close MPI
	return 0;	
}
