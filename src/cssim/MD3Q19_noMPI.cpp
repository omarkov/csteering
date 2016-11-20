#include <vector>

#include <iostream>

using namespace std;

#include "MD3Q19_noMPI.h"
#include "types.h"

MD3Q19a::MD3Q19a() 
{	
	// init Vector Model 
	//            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
	int x[19] = { 0, 1,-1, 0, 0, 0, 0, 1,-1, 1,-1, 1,-1, 1,-1, 0, 0, 0, 0};
    int y[19] = { 0, 0, 0, 1,-1, 0, 0, 1,-1,-1, 1, 0, 0, 0, 0, 1,-1, 1,-1};
	int z[19] = { 0, 0, 0, 0, 0, 1,-1, 0, 0, 0, 0, 1,-1,-1, 1, 1,-1,-1, 1};

	for (unsigned int i = 0; i < 19; i++) {
		e_x[i] = x[i];
		e_y[i] = y[i];
		e_z[i] = z[i];
	}

	cellDataModel = NULL;
}


MD3Q19a::~MD3Q19a()
{
	// if there is an older cellDataModel it must be deleted first
	if (cellDataModel) {
		delete[] cellDataModel;
		cellDataModel = NULL;
	}
}

// init the D3Q19 Model
void MD3Q19a::initModel(const Voxels& voxels, double stdRelaxationValue, double dens, double accel)
{
	//stdPressure = pressure;
	stdAcceleration = accel;
	stdDensity = dens;
	tau_inv = stdRelaxationValue;	
	
	//Tau Values for later distribution calculation
	double t0 = stdDensity / 3.0;
	double t1 = stdDensity / 18.0;
	double t2 = stdDensity / 36.0;

	max_x = voxels.size();
	max_y = voxels[0].size();
	max_z = voxels[0][0].size();
	
	if (cellDataModel)
        	delete cellDataModel;

	cellDataModel = new cellData[max_x * max_y * max_z];	
	pbcbuffer = new cellData[max_y * max_z];
	

	// testwise 
	cout << "Parameters" << endl;
	cout << "StdDensity: " << stdDensity << endl;
	cout << "t0: " << t0 << endl;
	cout << "t1: " << t1 << endl;
	cout << "t2: " << t2 << endl;
	//end testwise
	
	//init cellDataModel
	//setting standard values for accel density and pressure in fluid nodes
	cout << "...Starting init..." << endl;
	for (unsigned int z = 0; z < max_z ; z++) 
	{		
		for (unsigned int y = 0; y < max_y ; y++) 
		{
			for (unsigned int x = 0; x < max_x ; x++) 
			{
				cellData* currentCell = getCell(x,y,z);
				if (voxels[x][y][z])
				{
					//init all Solid Nodes with following values
					currentCell->solid = true;
					currentCell->density = 0;
				} else {
					//init all Fluid Node with the GUI values
					currentCell->solid = false;
					currentCell->density = stdDensity;
					double *values = currentCell->distributionValue;
										
					values[0] = t0;
					unsigned int i;
					for(i = 1; i <= 6; ++i) values[i] = t1;
					for(i = 7; i <= 18; ++i) values[i] = t2;
					
				}
			}
		}
	}
	
	for (unsigned int z = 0; z < max_z ; z++) 
	{		
		for (unsigned int y = 0; y < max_y ; y++) 
		{
			cellData* currentCell = getBufferCell(y,z);

			currentCell->solid = false;
			currentCell->density = stdDensity;
			double *values = currentCell->distributionValue;
								
			values[0] = t0;
			unsigned int i;
			for(i = 1; i <= 6; ++i) values[i] = t1;
			for(i = 7; i <= 18; ++i) values[i] = t2;
		}
	}	
	cout << "Init done!" << endl;
}

void MD3Q19a::collision () 
{	
	
	// loop for collision with dimension-1 to avoid trying to write values to nirvana
	for (unsigned int z = 0; z < max_z; z++)
	{		
		for (unsigned int y = 0; y < max_y; y++) 
		{
			for (unsigned int x = 0; x < max_x; x++) 
			{ 
				cellData* currentCell = getCell(x,y,z);
				
			  	if (z == 11 /*&& y == 8  && x == 8*/) 
			   	{
			   		//cout << "====================================================================\n";
			   	  	printf("%6.5f ", currentCell->density);
					if(x == max_x -1)
						cout << "\n";
						
					overall.counter++;
					overall.sum += currentCell->density;
					overall.speed_x += currentCell->mv_x;
					overall.speed_y += currentCell->mv_y;
					overall.speed_z += currentCell->mv_z;
			   	  	//*/
			   	  	/*
					cout << "density: " << currentCell->density << endl;
					cout << "mv_x: " << currentCell->mv_x << endl;
					cout << "mv_y: " << currentCell->mv_y << endl;
					cout << "mv_z: " << currentCell->mv_z << endl;
					
					for (int i = 0; i < 19; i++)
					{ cout << i << ": ";
						printf("%4.2f", currentCell->distributionValue[i]);
						cout  << " | "; }
					cout << endl;
					//*/
			   	}
				
				
				// if node is solid node do bounceback
				if (currentCell->solid)
				{
					// Bounceback 
					
					#define BOUNCEBACK(indexSource, indexDestination) \
					currentCell->distributionValue[indexDestination] = currentCell->distributionValue[indexSource]; \
					currentCell->distributionValue[indexSource] = currentCell->distributionValue[indexDestination]
					
					BOUNCEBACK(1, 2);
					BOUNCEBACK(3, 4);
					BOUNCEBACK(5, 6);
					BOUNCEBACK(7, 8);
					BOUNCEBACK(10, 9);
					BOUNCEBACK(11, 12);
					BOUNCEBACK(14, 13);
					BOUNCEBACK(15, 16);
					BOUNCEBACK(18, 17);
					
				} 
				else 
				{
				// if node is fluid node relaxation
				
					// compute the new density, the new acceleration for all fluid nodes
					// then put them into Pressure-Acceleration-Condition and
					// compute the new distribution according to the Model
					//######## D3Q19 ########
					//#	t0		t1		t3	#
					//#---------------------#
					//#	1/3		1/18   1/36 #
					//#######################
					
					//compute density
					//density = sum of all distribution values (1-19)

					double *values = currentCell->distributionValue;
					double dd = 0.;

					for (unsigned int i = 0; i < 19; i++)
					{
						dd += values[i];
					}
					currentCell->density = dd;
					
					//compute macroscopic acceleration
					//x-axis: (f[1]-f[2]+f[7]-f[8]+f[9]-f[10]+f[11]-f[12]+f[13]-f[14]
					//y-axis: (f[5]-f[6]+f[11]-f[12]-f[13]+f[14]+f[15]-f[16]-f[17]+f[18])
					//z-axis: (f[3]-f[4]+f[7]-f[8]-f[9]+f[10]+f[15]-f[16]+f[17]-f[18])
					//###################################################################
					
					// macroskopic Velocities mv_x, mv_y, mv_z
					
					double mv_x = (    values[1]  - values[2]  + values[7]  - values[8]  + values[9]  - values[10]
								+ values[11] - values[12] + values[13] - values[14] ) / dd;					

					double mv_y = (   values[5]  - values[6]  + values[11] - values[12] - values[13] + values[14]
								+ values[15] - values[16] - values[17] + values[18] ) / dd;

					double mv_z = (   values[3]	 - values[4]  + values[7]  - values[8]	- values[9]	 + values[10]
								+ values[15] - values[16] + values[17] - values[18] ) / dd;
								
					currentCell->mv_x = mv_x;
					currentCell->mv_y = mv_y;
					currentCell->mv_z = mv_z;
					
					//compute equilibrium distribution
					double square = (mv_x * mv_x) + (mv_y * mv_y) + (mv_z * mv_z);
					double f_eq[19];
					
					#define SQR(x) ((x) * (x))
					#define f0_i(cu) fix + (t_rho * (((cu) * 3.0) + (SQR((cu)) * 4.5)))
					
					// center
					// cu0 and cu0^2 is "0", thus just take the "fix" part
					f_eq[0] = (dd / 3.0) * (1.0 - (square * 1.5));
					
					// axis
					double t_rho = dd / 18.0;
					// "fix" is constant within each of the following two blocks
					double fix = t_rho * (1.0 - (square * 1.5));
					// 1|2, 3|4, .. .just differ in the "(cu) * 3.0" term 
					// (it is added in 1,3,5, ... but subtraced in 2,4,6,...)
					// -> just subtract it twice from the first result
					double t_rho_6 = 6.0 * t_rho;
					f_eq[1] = f0_i( mv_x);
					f_eq[2] = f_eq[1] - (t_rho_6 * mv_x);
					f_eq[3] = f0_i( mv_y);
					f_eq[4] = f_eq[3] - (t_rho_6 * mv_y);
					f_eq[5] = f0_i( mv_z);
					f_eq[6] = f_eq[5] - (t_rho_6 * mv_z);
					
					// diagonal
					t_rho = dd / 36.0;
					fix = t_rho * (1.0 - (square * 1.5));
					t_rho_6 = 6.0 * t_rho;
					f_eq[7] = f0_i(  mv_x + mv_y);
					f_eq[8] = f_eq[7] - (t_rho_6 * (mv_x + mv_y));
					f_eq[9] = f0_i(  mv_x - mv_y);
					f_eq[10] = f_eq[9] - (t_rho_6 * (mv_x - mv_y));
					f_eq[11] = f0_i( mv_x + mv_z);
					f_eq[12] = f_eq[11] - (t_rho_6 * (mv_x + mv_z));
					f_eq[13] = f0_i( mv_x - mv_z);
					f_eq[14] = f_eq[13] - (t_rho_6 * (mv_x - mv_z));
					f_eq[15] = f0_i( mv_y + mv_z);
					f_eq[16] = f_eq[15] - (t_rho_6 * (mv_y + mv_z));
					f_eq[17] = f0_i( mv_y - mv_z);
					f_eq[18] = f_eq[17] - (t_rho_6 * (mv_y - mv_z));
					
					#undef f0_i
					#undef SQR
					
					// relaxation
					for (int i=0; i<19; i++) {
						currentCell->distributionValue[i] =
						(currentCell->distributionValue[i] + tau_inv * (f_eq[i] - currentCell->distributionValue[i]));
					}
				}
			}
			//cout << "====================================" << endl;
		}
	}
	// usleep(50000);
	cout << "\x1b[2J";
}
	
void MD3Q19a::propagate ()
{
	cout << "propagation..." << endl;
	
	
	//new propagtion for testin purposes.
	//Propagate without copyfield
	// TtB:
	
	cellData* next;
	cellData* current;
	
	// backward
	for (int z = 1; z < max_z -1; z++) 
	{
		for (int y = 1; y < max_y -1; y++) 
		{
			for (int x = 0; x < max_x; x++) 
			{
				current = getCell(x, y, z);
				
				if (x == 0)
				{
					//fields going into input buffer
					// 2
					sendToLeftBuff(y, z, 2, current->distributionValue[2]);
					// 8
					sendToLeftBuff(y - 1, z, 8, current->distributionValue[8]);
					// 12
					sendToLeftBuff(y, z - 1, 12, current->distributionValue[12]);

				} else {
					// 2
					next = getCell(x - 1, y, z);
					next->distributionValue[2] = current->distributionValue[2];
					// 8
					next = getCell(x - 1, y - 1, z);
					next->distributionValue[8] = current->distributionValue[8];
					// 12
					next = getCell(x - 1, y, z - 1);
					next->distributionValue[12] = current->distributionValue[12];					
				}


				if (x == max_x - 1)
				{
					//fields going into output buffer
					// 9
					sendToRightBuff(y - 1, z, 9, current->distributionValue[9]);
					// 13
					sendToRightBuff(y, z - 1, 13, current->distributionValue[13]);


				} else {
					// 9
					next = getCell(x + 1, y - 1, z);
					next->distributionValue[9] = current->distributionValue[9];
					// 13
					next = getCell(x + 1, y, z - 1);
					next->distributionValue[13] = current->distributionValue[13];
				}

				// 4
				next = getCell(x, y - 1, z);
				next->distributionValue[4] = current->distributionValue[4];
				// 6
				next = getCell(x, y, z - 1);
				next->distributionValue[6] = current->distributionValue[6];
				// 16
				next = getCell(x, y - 1, z - 1);
				next->distributionValue[16] = current->distributionValue[16];
				// 17
				next = getCell(x, y + 1, z - 1);
				next->distributionValue[17] = current->distributionValue[17];
			}
		}
	}
	// forward
	for (int z = max_z -2; z > 0; z--) 
	{
		for (int y = max_y -2; y > 0; y--) 
		{
	    	for (int x = max_x -1; x >= 0; x--) 
	    	{
				current = getCell(x, y, z);

				if (x == max_x - 1)
				{
					//fields going into output buffer
					// 1
					sendToOutBuff(y, z, 1, current->distributionValue[1]);
					// 7
					sendToOutBuff(y + 1, z, 7,current->distributionValue[7]);
					// 11
					sendToOutBuff(y, z + 1, 11, current->distributionValue[11]);
					
				} else {
					// 1
					next = getCell(x + 1, y, z);
					next->distributionValue[1] = current->distributionValue[1];
					// 7
					next = getCell(x + 1, y + 1, z);
					next->distributionValue[7] = current->distributionValue[7];
					// 11
					next = getCell(x + 1, y, z + 1);
					next->distributionValue[11] = current->distributionValue[11];
				}
				if (x == 0)
				{
					//fields going into input buffer
					// 10
					sendToInBuff(y + 1, z, 10, current->distributionValue[10]);
					// 14
					sendToInBuff(y, z + 1, 14, current->distributionValue[14]);
					
				} else {
					// 10
					next = getCell(x - 1, y + 1, z);
					next->distributionValue[10] = current->distributionValue[10];
					// 14
					next = getCell(x - 1, y, z + 1);
					next->distributionValue[14] = current->distributionValue[14];
				}
				// 15
				next = getCell(x, y + 1, z + 1);
				next->distributionValue[15] = current->distributionValue[15];
				// 18
				next = getCell(x, y - 1, z + 1);
				next->distributionValue[18] = current->distributionValue[18];

				// 3
				next = getCell(x, y + 1, z);
				next->distributionValue[3] = current->distributionValue[3];
				// 5
				next = getCell(x, y, z + 1);
				next->distributionValue[5] = current->distributionValue[5];
	    	}
		}
	}	

	
	// propagate the buffer for next step
   	for (int z = max_z -1; z > 1; z--) 
	{
		for (int y = max_y -1; y > 1; y--) 
    	{

			current = getBufferCell(y, z);
			
			// 2
			next = getCell(max_x, y, z);
			next->distributionValue[2] = current->distributionValue[2];
			// 8
			next = getCell(max_x, y - 1, z);
			next->distributionValue[8] = current->distributionValue[8];
			// 12
			next = getCell(max_x, y, z - 1);
			next->distributionValue[12] = current->distributionValue[12];					
			// 10
			next = getCell(max_x, y + 1, z);
			next->distributionValue[10] = current->distributionValue[10];
			// 14
			next = getCell(max_x, y, z + 1);
			next->distributionValue[14] = current->distributionValue[14];
		
			// 1
			next = getCell(0, y, z);
			next->distributionValue[1] = current->distributionValue[1];
			// 7
			next = getCell(0, y + 1, z);
			next->distributionValue[7] = current->distributionValue[7];
			// 9
			next = getCell(0, y - 1, z);
			next->distributionValue[9] = current->distributionValue[9];
			// 11
			next = getCell(0, y, z + 1);
			next->distributionValue[11] = current->distributionValue[11];
			// 13
			next = getCell(0, y, z - 1);
			next->distributionValue[13] = current->distributionValue[13];


    	}
	}
	
	cout << "overall density " << (overall.sum / overall.counter) << "\t difference to last step: " << ((overall.sum / overall.counter) - (overall.sum_old / overall.counter)) << endl;
	cout << "overall speed x: " << (overall.speed_x / overall.counter) << endl;
	cout << "overall speed y: " << (overall.speed_y / overall.counter) << endl;
	cout << "overall speed z: " << (overall.speed_z / overall.counter) << endl;
	overall.sum_old = overall.sum;
	overall.sum = 0;

	overall.counter = 0;
	overall.speed_x = 0;
	overall.speed_y = 0;
	overall.speed_z = 0;
	
	cout << "..done!" << endl;

}

double MD3Q19a::getPressure(int x, int y, int z)
{
        cellData* currentCell = getCell(x,y,z);
        return currentCell->distributionValue[0];
   //     return 1.5;
}


void MD3Q19a::sendToLeftBuff (int y, int z, int field, double value)
{
	if (y >= 0 && z >= 0 && y < max_y && z < max_z) 
	{
		cellData* c =  getBufferCell(y , z);
		c->distributionValue[field] = value;	
	}
}

void MD3Q19a::sendToRightBuff (int y, int z, int field, double value)
{
	if (y >= 0 && z >= 0 && y < max_y && z < max_z) 
	{
		cellData* c =  getBufferCell(y , z);
		c->distributionValue[field] = value;	
	}
}

void MD3Q19a::redistribute()
{
	//process acceleration on first collumn
	
	double t1_accel = stdAcceleration * stdDensity / 19.;
	double t2_accel = stdAcceleration * stdDensity / 38.;
	
	#define REDIST(index, value) c->distributionValue[index] += (double) value
	
	
	for (unsigned int y = 1; y < max_y -1; y++) 
	{
		for (unsigned int z = 1; z < max_z -1; z++) 
		{
			cellData* c = getCell(0, y, z);
			
			if (c->distributionValue[2] - t1_accel > 0 &&
				c->distributionValue[8] - t2_accel > 0 && 
				c->distributionValue[10] - t2_accel > 0 && 
				c->distributionValue[12] - t2_accel > 0 && 
				c->distributionValue[14] - t2_accel > 0)
			{
				REDIST(1, +t1_accel);				
				REDIST(7, +t2_accel);
				REDIST(9, +t2_accel);
				REDIST(11, +t2_accel);
				REDIST(13, +t2_accel);

				REDIST(2, -t1_accel);
				REDIST(8, -t2_accel);
				REDIST(10, -t2_accel);
				REDIST(12, -t2_accel);
				REDIST(14, -t2_accel);
 			}
		}
	}	
	#undef REDIST
}


// ###################### interpolation ######################################
/* the interpolation is done by two bilinear interpolation followed by
 * a single linear interpolation.
 * it returns a float value.
 */
// interpolation for density
float SimCommunicator::interpolation(float x, float y, float z, interpolation_type type)
{
	double ipNeighValue1, ipNeighValue2,
		   ipNeighValue3, ipNeighValue4,
		   ipNeighValue5, ipNeighValue6, 
		   ipNeighValue7, ipNeighValue8;

	float xIntValue, yIntValue, zIntValue,
		  ipValueDown, ipValueUp, ipResult;
		  
	unsigned int xIntPoint, yIntPoint, zIntPoint;
	
// typecast of x, y, z
	xIntPoint = (unsigned int)x;
	yIntPoint = (unsigned int)y;
	zIntPoint = (unsigned int)z;

// cord factor needed for linear interpolation - there called "factor"
// the difference has to be multiplied  with that factors
	xFactor = x - xIntPoint; 
	yFactor = y - yIntPoint;
	zFactor = z - zIntPoint;

// neighbor values
// lower face points
/* 4 ---- 3
 * |      |
 * |      |
 * 1 ---- 2
 */
 // point 1
	cellData* currentCell = getCell(xIntPoint, yIntPoint, zIntPoint);
	ipNeighValue1 = getCellValue(currentCell, type);
// point 2
	cellData* currentCell = getCell(xIntPoint + 1, yIntPoint, zIntPoint);
	ipNeighValue2 = getCellValue(currentCell, type);
// point 3
	cellData* currentCell = getCell(xIntPoint + 1, yIntPoint + 1, zIntPoint);
	ipNeighValue3 = getCellValue(currentCell, type);
// point 4	
	cellData* currentCell = getCell(xIntPoint, yIntPoint + 1, zIntPoint);
	ipNeighValue4 = getCellValue(currentCell, type);
// upper face points
/* 8 ---- 7
 * |      |
 * |      |
 * 5 ---- 6
 */
// point 5
	cellData* currentCell = getCell(xIntPoint, yIntPoint, zIntPoint + 1);
	ipNeighValue5 = getCellValue(currentCell, type);
// point 6
	cellData* currentCell = getCell(xIntPoint + 1, yIntPoint, zIntPoint + 1);
	ipNeighValue6 = getCellValue(currentCell, type);
// point 7
	cellData* currentCell = getCell(xIntPoint + 1, yIntPoint + 1, zIntPoint + 1);
	ipNeighValue7 = getCellValue(currentCell, type);
// point 8
	cellData* currentCell = getCell(xIntPoint, yIntPoint + 1, zIntPoint + 1);
	ipNeighValue8 = getCellValue(currentCell, type);

// packing values into 2 arrays - upper face points = ipNeighValUp
//								lower face points = ipNeighValDown
	double ipNeighValUp []   = {ipNeighValue1, ipNeighValue2, ipNeighValue3, ipNeighValue4}; 
	double ipNeighValDown [] = {ipNeighValue5, ipNeighValue6, ipNeighValue7, ipNeighValue8};

//#### here the actually interpolation happens	
// compute two bilinear interpolations
	bilinearInterpolation(xFactor, yFactor, *ipNeighValDown);
	ipValueDown = ipResult;
	bilinearInterpolation(xFactor, yFactor, *ipNeighValUp);
	ipValueUp = ipResult;
// return the final value of the "trilinear interpolation"
	return (linearInterpolation(zFactor, ipValueDown, ipValueUp));
}

// interpolation for velocity
float SimCommunicator::getCellValue(const cellData& data, interpolation_type type)
{
	switch(type)
	{
		case INTERTYPE_DENSITY:
			return data->density;
		
		case INTERTYPE_MV_X:
			return data->mv_x;
		
		case INTERTYPE_MV_Y:
			return data->mv_y; 
		
		case INTERTYPE_MV_Z:
			return data->mv_z;
	}
}

// bilinear interpolation
/* compute two linear interpolations to get the values of alpha and beta
 * then compute the "bilinear" interpolation
 */ 
float SimCommunicator::bilinearInterpolation(float xFactor, float yFactor, &double ipNeighValues)
{	
	float alpha, beta, ipValue, ipValueUp, ipValueDown;
	
	alpha = linearInterpolation(xFactor, ipNeigValues[0], ipNeighValues[1]); 
	beta  = linearInterpolation(yFactor, ipNeigValues[2], ipNeighValues[3]);
	
	ipResult = (1 - alpha)(1-beta) * ipNeighValues[0] + alpha * (1 - beta) * ipNeighValues[1]
	+ (1 - alpha) * beta * ipNeighValues[3] + alpha * beta * ipNeighValues[2];
		
		return (ipResult);
}

// linear interpolation of 2 float values
/* if the cord (factor) is laying directly on voxel-center, it equals 
 * the value of the center (here valueLast)
 * if it is not, the value is computed by the difference of the two
 * values (valueLast and valueNext) divided by the factor.
 * the factor is 0 > 1.
 * afterwards the temp value is added to the smaller value to 
 * get the interpolation value
 */
float SimCommunicator::linearInterpolation(float factor, double valueLast, double valueNext)
{	
	if (factor == 0){
			ipValueLinear = valueLast; 
	}

	if (valueNext >= valueLast)
	{
		ipValueTemp = abs(valueNext - valueLast) * factor;		
		ipValueLinear = valueLast + ipValueTemp;
	}
	else
	{
		ipValueTemp = abs(valueNext - valueLast) * (1 - factor);	
		ipValueLinear = valueLast + ipValueTemp;
	}
	return (ipValueLinear);
}

// tracing the next points and calling the interpolation
void SimCommunicator::tracer(float x, float y, float z)
{
	double xSpeed, ySpeed, zSpeed;
	int intStepSize = 10;

// Wenn der hintere Rand erreicht ist muss schluss sein!!! ###########################
// !!!ACHTUNG !!! max_x noch nicht gesetzt
while (x < max_x){

// interpolate the value of the current point	
	pointValue = interpolation(x, y, z);
// return point and values to VIS	
	return(x, y, z, pointValue);	
			 
// get vectors	
	currentCell = getCell(x, y, z);
	xSpeed = currentCell -> mv_x;
	ySpeed = currentCell -> mv_y;
	zSpeed = currentCell -> mv_z;

// compute unit vector and touch stepSize for the next interpolation step
	xSpeed = (xSpeed / 3) / intStepSize;
	ySpeed = (ySpeed / 3) / intStepSize;
	zSpeed = (zSpeed / 3) / intStepSize;	

// successor point
	x = x + xSpeed;
	y = y + ySpeed;
	z = z + zSpeed;	
	}
}

// plain calculation
float SimCommunicator::plainMaker(int intFactor)
{
	//einlesen der ersten 3 Punkte, dann die Abstände zwischen 1 und 2 und 
	//eine zwischen 2 und 3 bilden, dann die Abstände in lamda geforderter
	//Genauigkeit durchlaufen und bei jedem step interpolation aufrufen
	xPoint1 = ;
	yPoint1 = ;
	zPoint1 = ;
	xPoint2 = ;
	yPoint2 = ;
	zPoint2 = ;
	xPoint3 = ;
	yPoint3 = ;
	zPoint3 = ;
// distace between p1, p2 and p2,p3
	xdistanceP1P2 = (xPoint2 - xPoint1);
	ydistanceP1P2 = (yPoint2 - yPoint1);
	zdistanceP1P2 = (zPoint2 - zPoint1);
	xdistanceP2P3 = (xPoint3 - xPoint2);
	ydistanceP2P3 = (yPoint3 - yPoint2);
	zdistanceP2P3 = (zPoint3 - zPoint2);
// calculate the step size with the input from the interpolation factor	
	xStepP1P2 = xdistanceP1P2 / intFactor;
	yStepP1P2 = yDistanceP1P2 / intFactor;
	zStepP1P2 = zdistanceP1P2 / intFactor;
	xStepP2P3 = xdistanceP2P3 / intFactor;
	yStepP2P3 = yDistanceP2P3 / intFactor;
	zStepP2P3 = zdistanceP2P3 / intFactor;	
// 
		{
			plainIntValue = interpolation(xPoint1, yPoint1, zPoint1);
			return(xPoint1, yPoint1, zPoint1, plainIntValue);
			xPoint1 = xPoint1 + xStepP1P2;
			yPoint1 = yPoint1 + yStepP1P2;
			zPoint1 = zPoint1 + zStepP1P2;
		}
}