#include <vector>
#include <iostream>

using namespace std;

#include "MD3Q19a.h"
#include "types.h"
#include "mpitags.h"

#define LEFT 	0
#define RIGHT	1

MD3Q19a::MD3Q19a() 
{
	// init Vector Model 
	//            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
	int x[19] = { 0, 1,-1, 0, 0, 0, 0, 1,-1, 1,-1, 1,-1, 1,-1, 0, 0, 0, 0};
    int y[19] = { 0, 0, 0, 1,-1, 0, 0, 1,-1,-1, 1, 0, 0, 0, 0, 1,-1, 1,-1};
	int z[19] = { 0, 0, 0, 0, 0, 1,-1, 0, 0, 0, 0, 1,-1,-1, 1, 1,-1,-1, 1};

	myrank = MPI::COMM_WORLD.Get_rank();
	nprocs = MPI::COMM_WORLD.Get_size();

	if (myrank == 1)
	{
		leftNB = nprocs - 1;
	}
	else 
	{
		leftNB = myrank - 1;
	}
	
	if (myrank == (nprocs - 1))
	{
		rightNB = 1;
	}
	else 
	{
		rightNB = myrank + 1;
	}
	
	for (unsigned int i = 0; i < 19; i++) {
		e_x[i] = x[i];
		e_y[i] = y[i];
		e_z[i] = z[i];
	}

/*	if (cellDataModel)
        	delete cellDataModel;  */

	cellDataModel = NULL;
	//cout << "slave spawned on Sim " << myrank << endl;

	listenForCommand();
}


MD3Q19a::~MD3Q19a()
{
	// if there is an older cellDataModel it must be deleted first
	if (cellDataModel) {
		delete[] cellDataModel;
		cellDataModel = NULL;
	}
}

void MD3Q19a::collision () 
{
	// loop for collision with dimension-1 to avoid trying to write values to nirvana
	for (unsigned int z = 1; z < max_z; z++) 
	{		
		for (unsigned int y = 1; y < max_y; y++) 
		{
			for (unsigned int x = 1; x < max_x; x++)
			{ 
				cellData* currentCell = getCell(x,y,z);
								
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
					double density = 0.;

					for (unsigned int i = 0; i < 19; i++)
					{
						density += values[i];
					}
					currentCell->density = (double)density;
					
					//compute macroscopic acceleration
					//x-axis: (f[1]-f[2]+f[7]-f[8]+f[9]-f[10]+f[11]-f[12]+f[13]-f[14]
					//y-axis: (f[5]-f[6]+f[11]-f[12]-f[13]+f[14]+f[15]-f[16]-f[17]+f[18])
					//z-axis: (f[3]-f[4]+f[7]-f[8]-f[9]+f[10]+f[15]-f[16]+f[17]-f[18])
					//###################################################################
					
					// macroskopic Velocities mv_x, mv_y, mv_z
					
					double mv_x =    values[1]  - values[2]  + values[7]  - values[8]  + values[9]  - values[10]
								+ values[11] - values[12] + values[13] - values[14];					

					double mv_y =    values[5]  - values[6]  + values[11] - values[12] - values[13] + values[14]
								+ values[15] - values[16] - values[17] + values[18];

					double mv_z =    values[3]	 - values[4]  + values[7]  - values[8]	- values[9]	 + values[10]
								+ values[15] - values[16] - values[17]
								+ values[18];
							
					mv_x /= density;
					mv_y /= density;
					mv_z /= density;
					
					currentCell->mv_x = mv_x;
					currentCell->mv_y = mv_y;
					currentCell->mv_z = mv_z;
					
					//compute equilibrium distribution
					double square = mv_x * mv_x + mv_y * mv_y + mv_z * mv_z;
					double f_eq[19];
					
					#define SQR(x) ((x) * (x))
					#define f0_i(cu) fix + (t_rho * (((cu) * 3.0) + (SQR((cu)) * 4.5)))
					
					// center
					// cu0 and cu0^2 is "0", thus just take the "fix" part
					f_eq[0] = (density / 3.0) * (1.0 - (square * 1.5));
					
					// axis
					double t_rho = density / 18.0;
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
					t_rho = density / 36.0;
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
						currentCell->distributionValue[i] = (double) 
						(currentCell->distributionValue[i] + tau_inv * (f_eq[i] - currentCell->distributionValue[i]));
					}
				}
			}
			//cout << "====================================" << endl;
		}
	}
	// cout << "\x1b[2J";
	cout << "\t\t\t Collision..done! Waiting for synch..." << endl;
	MPI::COMM_WORLD.Barrier();
}
	
void MD3Q19a::propagate ()
{
	cout << "\t\t\tpropagation...synch" << endl;
	MPI::COMM_WORLD.Barrier();
	
	
	//new propagtion for testin purposes.
	//Propagate without copyfield
	// TtB:
	
	cellData* next;
	cellData* current;
	
	// backward
	for (unsigned int z = 1; z < max_z; z++) 
	{
		for (unsigned int y = 1; y < max_y; y++) 
		{
			for (unsigned int x = 0; x < max_x; x++) 
			{
				current = getCell(x, y, z);
				
//				cout << x << ":" << y << ":" << z << "|";
//				cout << current->solid << "|";			
				if (x == 0)
				{
					//fields going into left buffer
					// 2
					next = getBCell(LEFT, y, z);
					next->distributionValue[2] = current->distributionValue[2];
					
					// 8
					next = getBCell(LEFT, y - 1, z);
					next->distributionValue[8] = current->distributionValue[8];

					// 12
					next = getBCell(LEFT, y, z - 1);
					next->distributionValue[12] = current->distributionValue[12];										

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


				if (x == max_x -1)
				{
					// 9
					next = getBCell(RIGHT, y - 1, z);
					next->distributionValue[9] = current->distributionValue[9];
					// 13
					next = getBCell(RIGHT, y, z - 1);
					next->distributionValue[13] = current->distributionValue[13];

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
	for (unsigned int z = max_z -2; z > 0; z-- ) 
	{
		for (unsigned int y = max_y -2; y > 0; y-- ) 
		{
	    	for (int x = max_x -1; x >= 0; x-- ) 
	    	{
				current = getCell(x, y, z);
//				cout << x << ":" << y << ":" << z << "|";
//				cout << current->solid << "|";			

				if ((unsigned int)x == max_x-1)
				{
					//fields going into right buffer
					// 1
					next = getBCell(RIGHT, y, z);
					next->distributionValue[1] = current->distributionValue[1];
					// 7
					next = getBCell(RIGHT, y + 1, z);
					next->distributionValue[7] = current->distributionValue[7];
					// 11
					next = getBCell(RIGHT, y, z + 1);
					next->distributionValue[11] = current->distributionValue[11];
					
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
					//fields going into LEFT buffer
					// 10
					next = getBCell(LEFT, y + 1, z);
					next->distributionValue[10] = current->distributionValue[10];
					// 14
					next = getBCell(LEFT, y, z + 1);
					next->distributionValue[14] = current->distributionValue[14];
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
	
	//call the redistribution/acceleration in the first slice
	if (myrank == 1)
	{
		accelerate();	
	}
	
	cout << "\t\t\t Propagation..done! Waiting for synch..." << endl;
	MPI::COMM_WORLD.Barrier();
	
	//let even ranks send Buffer first and odd receive
	if ((myrank & 1) == 0)
	{
//		cout << "\t\t\tSim " << myrank << " EVEN" << endl;
		for (unsigned int y = 1; y < max_y - 1; y++)
		{
			for (unsigned int z = 1; z < max_z - 1; z++)
			{
				current = getBCell(LEFT, y, z);
				//fields going to left domain
				// 2
				sendToLeftBuff(y, z, 2, current->distributionValue[2]);
				// 8
				sendToLeftBuff(y - 1, z, 8, current->distributionValue[8]);
				// 10
				sendToLeftBuff(y + 1, z, 10, current->distributionValue[10]);
				// 12
				sendToLeftBuff(y, z - 1, 12, current->distributionValue[12]);
				// 14
				sendToLeftBuff(y, z + 1, 14, current->distributionValue[14]);
	
	
				current = getBCell(RIGHT, y, z);
				//fields going to right domain
				// 1
				sendToRightBuff(y, z, 1, current->distributionValue[1]);
				// 7
				sendToRightBuff(y + 1, z, 7,current->distributionValue[7]);
				// 9
				sendToRightBuff(y - 1, z, 9, current->distributionValue[9]);
				// 11
				sendToRightBuff(y, z + 1, 11, current->distributionValue[11]);
				// 13
				sendToRightBuff(y, z - 1, 13, current->distributionValue[13]);				
				
			}
		}

		MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, rightNB, MPI_Prop_Done);		
		MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, leftNB, MPI_Prop_Done);		
	}
	else
	{
//		cout << "\t\t\tSim " << myrank << " ODD" << endl;

		int receiving = 2;
		while(receiving)
		{
			// cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
			//receive commands or data
			MPI::COMM_WORLD.Recv(&data, sizeof(data), MPI::BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, status);
			
			 // cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
			//decide what to do by analysing the status tag
			switch (status.Get_tag())
			{
				case MPI_Data_IncLeft:
				
//					MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);
					getCell(0, data.y, data.z)->distributionValue[data.field] = data.value;
					break;
				
					
				case MPI_Data_IncRight:
				
//					MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);
					getCell(max_x, data.y, data.z)->distributionValue[data.field] = data.value;
					break;
					
				case MPI_Prop_Done:
					receiving --;
					break;
			}
		}		
	}	

	cout << "\t\t\t EVEN Propagation..done! Waiting for synch..." << endl;
	MPI::COMM_WORLD.Barrier();

	//let odd ranks send Buffer now and even receive
	if ((myrank & 1) != 0)
	{
		for (unsigned int y = 1; y < max_y - 1; y++)
		{
			for (unsigned int z = 1; z < max_z - 1; z++)
			{
				current = getBCell(LEFT, y, z);
				//fields going to left domain
				// 2
				sendToLeftBuff(y, z, 2, current->distributionValue[2]);
				// 8
				sendToLeftBuff(y - 1, z, 8, current->distributionValue[8]);
				// 10
				sendToLeftBuff(y + 1, z, 10, current->distributionValue[10]);
				// 12
				sendToLeftBuff(y, z - 1, 12, current->distributionValue[12]);
				// 14
				sendToLeftBuff(y, z + 1, 14, current->distributionValue[14]);
	
	
				current = getBCell(RIGHT, y, z);
				//fields going to right domain
				// 1
				sendToRightBuff(y, z, 1, current->distributionValue[1]);
				// 7
				sendToRightBuff(y + 1, z, 7,current->distributionValue[7]);
				// 9
				sendToRightBuff(y - 1, z, 9, current->distributionValue[9]);
				// 11
				sendToRightBuff(y, z + 1, 11, current->distributionValue[11]);
				// 13
				sendToRightBuff(y, z - 1, 13, current->distributionValue[13]);				
				
			}
		}
		MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, rightNB, MPI_Prop_Done);		
		MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, leftNB, MPI_Prop_Done);		
	}
	else
	{
		int receiving = 2;
		while(receiving)
		{
			//receive commands or data
			MPI::COMM_WORLD.Recv(&data, sizeof(data), MPI::BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, status);
			
			//decide what to do by analysing the status tag
			switch (status.Get_tag())
			{
				case MPI_Data_IncLeft:
					getCell(0, data.y, data.z)->distributionValue[data.field] = data.value;
					break;

				case MPI_Data_IncRight:
					getCell(max_x, data.y, data.z)->distributionValue[data.field] = data.value;
					break;
					
				case MPI_Prop_Done:
					receiving --;
					break;
			}
		}				
	}	

	cout << "\t\t\t ODD Propagation..done! Waiting for synch..." << endl;
	MPI::COMM_WORLD.Barrier();
}

void MD3Q19a::accelerate()
{
	//process acceleration on sim 1 left buffer
	
	for (unsigned int y = 0; y < max_y; y++) 
	{
		for (unsigned int z = 0; z < max_z; z++) 
		{
			cellData* c = getBCell(0, y, z);
			
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

void MD3Q19a::sendToLeftBuff (int y, int z, int field, double value)
{
//	cout << myrank << " sending to " << leftNB << ", " << y << ":" << z << " field: " << field << " = " << value << endl;
	data.y = y; 
	data.z = z; 
	data.field = field; 
	data.value = value;
	int datasize = sizeof(data);
	MPI::COMM_WORLD.Send(&data, datasize, MPI::BYTE, leftNB, MPI_Data_IncRight);
}

void MD3Q19a::sendToRightBuff (int y, int z, int field, double value)
{
//	cout << myrank << "sending to " << rightNB << ", " << y << ":" << z << " field: " << field << " = " << value << endl;
	data.y = y; 
	data.z = z; 
	data.field = field; 
	data.value = value;
	int datasize = sizeof(data);

	MPI::COMM_WORLD.Send(&data, datasize, MPI::BYTE, rightNB, MPI_Data_IncLeft);
}

void MD3Q19a::listenForCommand()
{
	running = true;
	
	while (running)
	{

		cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
		//receive commands or data
		MPI::COMM_WORLD.Recv(&data, sizeof(data), MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);
		
		cout << "\t\t\t MSG Received...(" << myrank << ") " << status.Get_tag() << endl;
		//decide what to do by analysing the status tag
		switch (status.Get_tag())
		{
			case MPI_Disconnect:
			{
				running = false;
				break;
			}	
			case MPI_Set_Area:
			{
				MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
				waitForArea();
				cout << "\t\t\t Sim " << myrank << " waiting for synch..." << endl;
				MPI::COMM_WORLD.Barrier();
				cout << "\t Sim " << myrank << " synched!";
				break;
			}
			case MPI_Update_Enviroment:
			{
				MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
				enviroment envi;
				envi.accel = 0.;
				envi.dense = 0.;
				envi.relax = 0.;
				
				MPI::COMM_WORLD.Recv(&envi, sizeof(envi), MPI::BYTE, OVERMIND, MPI_Update_Enviroment, status);
				if ((status.Get_tag() == MPI_Update_Enviroment) && (status.Get_source() == OVERMIND ))
				{
					MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);				
				}
				tau_inv = stdRelaxationValue = envi.relax;
				stdDensity = envi.dense;
				stdAcceleration = envi.accel;
				
				//Tau Values for later distribution calculation
				t0 = stdDensity / 3.0;
				t1 = stdDensity / 18.0;
				t2 = stdDensity / 36.0;
				
				t1_accel = stdAcceleration * stdDensity / 19.;
				t2_accel = stdAcceleration * stdDensity / 38.;
							
				
				cout << "\t\t\t Sim " << myrank << " waiting for synch..." << endl;
				MPI::COMM_WORLD.Barrier();
				cout << "\t Sim " << myrank << " synched!";
				break;
			}
			case MPI_Model_Propagate:
			{
				// sleep(10);
				MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
				propagate();
				break;
			}				
			case MPI_Model_Collision:
			{
				MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
				collision();
				break;
			}			
			case MPI_Get_Cell:
			{
				cellData *cell;
				cell = getCell(data.x, data.y, data.z);
				MPI::COMM_WORLD.Send(&cell, sizeof(cell), MPI::BYTE, OVERMIND, MPI_Get_Cell);
				break;
			}			
			default:
			{
				cout << "\t\t\t Error receiving Message from: " << status.Get_source() << endl;
				cout << "\t\t\t Received unknown Tag: " << status.Get_tag() << endl;
				break;
			}
		};
	}
	cout << "\t\t\t exiting listener!" << endl;
	return;
}

void MD3Q19a::waitForArea()
{
	bool receiving = true;
	
	//Tau Values for later distribution calculation
	double t0 = stdDensity / 3.0;
	double t1 = stdDensity / 18.0;
	double t2 = stdDensity / 36.0;
	
	simField field;
	MPI::COMM_WORLD.Recv(&field, sizeof(field), MPI::BYTE, MPI_ANY_SOURCE, MPI_Set_Area + 1, status);

	cout << "\t\t\t MSG Received...(" << myrank << ") " << status.Get_tag() << endl;
	
	max_x = field.dim_x;
	max_y = field.dim_y;
	max_z = field.dim_z;
	cellDataModel = new cellData[max_x * max_y * max_z];

	while(receiving)
	{
		bufferdata data;
		//cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
		//receive commands or data
		MPI::COMM_WORLD.Recv(&data, sizeof(data), MPI::BYTE, OVERMIND, MPI_ANY_TAG ,status);
		
		//cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
		//decide what to do by analysing the status tag
		switch (status.Get_tag())
		{
			case MPI_Field:
				getCell(data.x, data.y, data.z)->solid = 1;
				break;
							
			case MPI_Field_Done:
				receiving = false;
				break;
		}
	}		

	cellData* currentCell;
	for (unsigned int x = 0; x < max_x; x++) 
	{
		for (unsigned int y = 0; y < max_y; y++) 
		{
			for (unsigned int z = 0; z < max_z; z++) 
			{
				currentCell = getCell(x,y,z);
				if (currentCell->solid == 0)
				{
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
	
	allocateBuffers();
	
//	MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);
	cout << "\t\t\t Area Set: Dimension, " << max_x << ":" << max_y << ":" << max_z << endl;
}

void MD3Q19a::allocateBuffers()
{
	if (bufferLeft) delete[] bufferLeft;
	if (bufferRight) delete[] bufferRight;
	bufferLeft = new cellData[(max_y + 2) * (max_z + 2)];
	bufferRight = new cellData[(max_y + 2) * (max_z + 2)];
}
