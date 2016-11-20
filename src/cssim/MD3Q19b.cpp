#include <vector>
#include <iostream>

using namespace std;

#include "MD3Q19b.h"
#include "types.h"
#include "mpitags.h"
#include "values.h"

#define LEFT 	0
#define RIGHT	1

#define INITSIZE 10000

MD3Q19b::MD3Q19b()
{
    bufferLeft = NULL;
    bufferRight = NULL;
    buffer = NULL;

    sendBuffer.bufferSize = INITSIZE;
    sendBuffer.size = 0;
    sendBuffer.probes = new simProbe[INITSIZE];
    receiveBuffer.bufferSize = INITSIZE;
    receiveBuffer.size = 0;
    receiveBuffer.cells = new bufferdata[INITSIZE];

    // init Vector Model 
    //            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
    int x[19] = { 0, 1, -1, 0, 0, 0, 0, 1, -1, 1, -1, 1, -1, 1, -1, 0, 0, 0, 0 };
    int y[19] = { 0, 0, 0, 1, -1, 0, 0, 1, -1, -1, 1, 0, 0, 0, 0, 1, -1, 1, -1 };
    int z[19] = { 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 1, -1, -1, 1, 1, -1, -1, 1 };

    myrank = MPI::COMM_WORLD.Get_rank();
    nprocs = MPI::COMM_WORLD.Get_size();


    outOfBounds = new cellData();
    initCell(outOfBounds);

    outOfBounds->density = 0.0f;

    if (myrank == 1) {
        leftNB = nprocs - 1;
    } else {
        leftNB = myrank - 1;
    }

    if (myrank == (nprocs - 1)) {
        rightNB = 1;
    } else {
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


MD3Q19b::~MD3Q19b()
{
    // if there is an older cellDataModel it must be deleted first
    if (cellDataModel) {
        delete[]cellDataModel;
        cellDataModel = NULL;
    }
}

void MD3Q19b::filter()
{
    bool receiving = true;
    int size = 0;

    while (receiving) {
        //cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
        //receive commands or data
        MPI::COMM_WORLD.Probe(OVERMIND, MPI_ANY_TAG, status);

        //cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
        //decide what to do by analysing the status tag
        switch (status.Get_tag()) {
        case MPI_Get_Cell:
            {
                simCoord cell;
                MPI::COMM_WORLD.Recv(&cell, sizeof(simCoord), MPI::BYTE, OVERMIND, MPI_Get_Cell, status);

                cellData *data;
                data = getCell(cell.x, cell.y, cell.z);
                simProbe probe;
                probe.solid = data->solid;
                probe.density = data->density;
                probe.mv_x = data->mv_x;
                probe.mv_y = data->mv_y;
                probe.mv_z = data->mv_z;

                MPI::COMM_WORLD.Send(&probe, sizeof(simProbe), MPI::BYTE, OVERMIND, MPI_Get_Cell);

            }
            break;
        case MPI_Set_Req_Size:
            {
                MPI::COMM_WORLD.Recv(&size, sizeof(int), MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);

                if (size > receiveBuffer.bufferSize) {
                    delete[]receiveBuffer.cells;
                    receiveBuffer.cells = new bufferdata[size];
                    receiveBuffer.bufferSize = size;
                    receiveBuffer.size = 0;
                }

                MPI::COMM_WORLD.Recv(receiveBuffer.cells, sizeof(bufferdata) * size, MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);

                MPI::COMM_WORLD.Send(&minmax, sizeof(minmax_t), MPI::BYTE, OVERMIND, MPI_Send_MinMax);

                receiveBuffer.size = size;

                if (size > sendBuffer.bufferSize) {
                    delete[]sendBuffer.probes;
                    sendBuffer.probes = new simProbe[size];
                    sendBuffer.bufferSize = size;
                }
                sendBuffer.size = 0;

                for (; sendBuffer.size < receiveBuffer.size; sendBuffer.size++) {
                    bufferdata *data = &receiveBuffer.cells[sendBuffer.size];
                    cellData *cell;
                    cell = getCell(data->x, data->y, data->z);

                    simProbe *sendProbe = &sendBuffer.probes[sendBuffer.size];
                    sendProbe->density = cell->density;
                    sendProbe->solid = cell->solid;
                    sendProbe->mv_x = cell->mv_x;
                    sendProbe->mv_y = cell->mv_y;
                    sendProbe->mv_z = cell->mv_z;
                }

                MPI::COMM_WORLD.Send(&sendBuffer.size, sizeof(int), MPI::BYTE, OVERMIND, MPI_Set_Resp_Size);

                MPI::COMM_WORLD.Send(sendBuffer.probes, sizeof(simProbe) * sendBuffer.size, MPI::BYTE, OVERMIND, MPI_Get_Cells);
                break;
            }
        case MPI_Filter_Done:
            {
                MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);
                receiving = false;
                break;
            }
        }
    }

    // cout << "\t\t\t Filter..done! Waiting for synch..." << endl;
    MPI::COMM_WORLD.Barrier();

}

void MD3Q19b::collision()
{
    minmax.min_density = MAXFLOAT;
    minmax.max_density = MINFLOAT;
    minmax.min_v = MAXFLOAT;
    minmax.max_v = MINFLOAT;

    // loop for collision with dimension-1 to avoid trying to write values to nirvana
    for (unsigned int z = 0; z < max_z; z++) {
        for (unsigned int y = 0; y < max_y; y++) {
            for (unsigned int x = 0; x < max_x; x++) {
                cellData *currentCell = getCell(x, y, z);

                // if node is solid node do bounceback
                if (currentCell->solid) {
                    // Bounceback 

                    for (int i = 1; i < 18; i += 2) {
                        double t = currentCell->distributionValue[i];
                        currentCell->distributionValue[i] = currentCell->distributionValue[i + 1];
                        currentCell->distributionValue[i + 1] = t;
                    }
                } else {
                    double *values = currentCell->distributionValue;
                    double density = 0.;

                    for (unsigned int i = 0; i < 19; i++) {
                        density += values[i];
                    }
                    currentCell->density = (double) density;

                    // macroskopic Velocities mv_x, mv_y, mv_z
                    double mv_x = ((values[V1] + values[V7] + values[V9] + values[V11] + values[V13]) - (values[V2] + values[V8] + values[V10] + values[V12] + values[V14])) / density;

                    double mv_y = ((values[V3] + values[V10] + values[V7] + values[V15] + values[V17]) - (values[V4] + values[V9] + values[V8] + values[V16] + values[V18])) / density;

                    double mv_z = ((values[V5] + values[V14] + values[V11] + values[V15] + values[V18]) - (values[V6] + values[V13] + values[V12] + values[V16] + values[V17])) / density;


                    currentCell->mv_x = mv_x;
                    currentCell->mv_y = mv_y;
                    currentCell->mv_z = mv_z;

                    //compute equilibrium distribution
                    double square = mv_x * mv_x + mv_y * mv_y + mv_z * mv_z;
                    double f_eq[19];

                    if (!currentCell->solid) {
                        if (density < minmax.min_density)
                            minmax.min_density = density;
                        if (density > minmax.max_density)
                            minmax.max_density = density;
                        if (square < minmax.min_v)
                            minmax.min_v = square;
                        if (square > minmax.max_v)
                            minmax.max_v = square;
                    }
#define SQR(x) ((x) * (x))
#define f0_i(cu) fix + (t_rho * (((cu) * 3.0) + (SQR((cu)) * 4.5)))

                    // center
                    // cu0 and cu0^2 is "0", thus just take the "fix" part
                    f_eq[V0] = (density / 3.0) * (1.0 - (square * 1.5));

                    // axis
                    double t_rho = density / 18.0;
                    // "fix" is constant within each of the following two blocks
                    double fix = t_rho * (1.0 - (square * 1.5));
                    // 1|2, 3|4, .. .just differ in the "(cu) * 3.0" term 
                    // (it is added in 1,3,5, ... but subtraced in 2,4,6,...)
                    // -> just subtract it twice from the first result
                    double t_rho_6 = 6.0 * t_rho;
                    f_eq[V1] = f0_i(mv_x);
                    f_eq[V2] = f_eq[V1] - (t_rho_6 * mv_x);
                    f_eq[V3] = f0_i(mv_y);
                    f_eq[V4] = f_eq[V3] - (t_rho_6 * mv_y);
                    f_eq[V5] = f0_i(mv_z);
                    f_eq[V6] = f_eq[V5] - (t_rho_6 * mv_z);

                    // diagonal
                    t_rho = density / 36.0;
                    fix = t_rho * (1.0 - (square * 1.5));
                    t_rho_6 = 6.0 * t_rho;
                    f_eq[V7] = f0_i(mv_x + mv_y);
                    f_eq[V8] = f_eq[V7] - (t_rho_6 * (mv_x + mv_y));
                    f_eq[V9] = f0_i(mv_x - mv_y);
                    f_eq[V10] = f_eq[V9] - (t_rho_6 * (mv_x - mv_y));
                    f_eq[V11] = f0_i(mv_x + mv_z);
                    f_eq[V12] = f_eq[V11] - (t_rho_6 * (mv_x + mv_z));
                    f_eq[V13] = f0_i(mv_x - mv_z);
                    f_eq[V14] = f_eq[V13] - (t_rho_6 * (mv_x - mv_z));
                    f_eq[V15] = f0_i(mv_y + mv_z);
                    f_eq[V16] = f_eq[V15] - (t_rho_6 * (mv_y + mv_z));
                    f_eq[V17] = f0_i(mv_y - mv_z);
                    f_eq[V18] = f_eq[V17] - (t_rho_6 * (mv_y - mv_z));

#undef f0_i
#undef SQR

                    // relaxation
                    for (int i = 0; i < 19; i++) {
                        currentCell->distributionValue[i] = (double)
                            (currentCell->distributionValue[i] + tau_inv * (f_eq[i] - currentCell->distributionValue[i]));
                    }
                }
            }
            //cout << "====================================" << endl;
        }
    }
    // cout << "\x1b[2J";
    // cout << "\t\t\t Collision..done! Waiting for synch..." << endl;
    MPI::COMM_WORLD.Barrier();
}

void MD3Q19b::propagate()
{
    // cout << "\t\t\tpropagation...synch" << endl;
    MPI::COMM_WORLD.Barrier();


    //new propagtion for testin purposes.
    //Propagate without copyfield
    // TtB:

    cellData *next;
    bufferData *bnext;
    cellData *current;

    // backward
    for (unsigned int z = 1; z < max_z; z++) {
        for (unsigned int y = 1; y < max_y; y++) {
            for (unsigned int x = 0; x < max_x; x++) {
                current = getCell(x, y, z);

//                              cout << x << ":" << y << ":" << z << "|";
//                              cout << current->solid << "|";                  
                if (x == 0) {
                    //fields going into left buffer
                    // 2
                    bnext = getBCell(LEFT, y, z);
                    bnext->distributionValue[B2] = current->distributionValue[V2];

                    // 8
                    bnext = getBCell(LEFT, y - 1, z);
                    bnext->distributionValue[B8] = current->distributionValue[V8];
                    // 12
                    bnext = getBCell(LEFT, y, z - 1);
                    bnext->distributionValue[B12] = current->distributionValue[V12];

                } else {
                    // 2
                    next = getCell(x - 1, y, z);
                    next->distributionValue[V2] = current->distributionValue[V2];
                    // 8
                    next = getCell(x - 1, y - 1, z);
                    next->distributionValue[V8] = current->distributionValue[V8];
                    // 12
                    next = getCell(x - 1, y, z - 1);
                    next->distributionValue[V12] = current->distributionValue[V12];
                }


                if (x == max_x - 1) {
                    // 9
                    bnext = getBCell(RIGHT, y - 1, z);
                    bnext->distributionValue[B9] = current->distributionValue[V9];
                    // 13
                    bnext = getBCell(RIGHT, y, z - 1);
                    bnext->distributionValue[B13] = current->distributionValue[V13];

                } else {
                    // 9
                    next = getCell(x + 1, y - 1, z);
                    next->distributionValue[V9] = current->distributionValue[V9];

                    // 13
                    next = getCell(x + 1, y, z - 1);
                    next->distributionValue[V13] = current->distributionValue[V13];
                }

                // 4
                next = getCell(x, y - 1, z);
                next->distributionValue[V4] = current->distributionValue[V4];
                // 6
                next = getCell(x, y, z - 1);
                next->distributionValue[V6] = current->distributionValue[V6];
                // 16
                next = getCell(x, y - 1, z - 1);
                next->distributionValue[V16] = current->distributionValue[V16];
                // 17
                next = getCell(x, y + 1, z - 1);
                next->distributionValue[V17] = current->distributionValue[V17];
            }
        }
    }

    // forward
    for (int z = (int)max_z - 2; z >= 0; z--) {
        for (int y = (int)max_y - 2; y >= 0; y--) {
            for (int x = (int)max_x - 1; x >= 0; x--) {
                current = getCell(x, y, z);

//                              cout << x << ":" << y << ":" << z << "|";
//                              cout << current->solid << "|";                  

                if ((unsigned int) x == max_x - 1) {
                    //fields going into right buffer
                    // 1
                    bnext = getBCell(RIGHT, y, z);
                    bnext->distributionValue[B1] = current->distributionValue[V1];
                    // 7
                    bnext = getBCell(RIGHT, y + 1, z);
                    bnext->distributionValue[B7] = current->distributionValue[V7];
                    // 11
                    bnext = getBCell(RIGHT, y, z + 1);
                    bnext->distributionValue[B11] = current->distributionValue[V11];

                } else {
                    // 1
                    next = getCell(x + 1, y, z);
                    next->distributionValue[V1] = current->distributionValue[V1];
                    // 7
                    next = getCell(x + 1, y + 1, z);
                    next->distributionValue[V7] = current->distributionValue[V7];
                    // 11
                    next = getCell(x + 1, y, z + 1);
                    next->distributionValue[V11] = current->distributionValue[V11];
                }
                if (x == 0) {
                    //fields going into LEFT buffer
                    // 10
                    bnext = getBCell(LEFT, y + 1, z);
                    bnext->distributionValue[B10] = current->distributionValue[V10];
                    // 14
                    bnext = getBCell(LEFT, y, z + 1);
                    bnext->distributionValue[B14] = current->distributionValue[V14];
                } else {
                    // 10
                    next = getCell(x - 1, y + 1, z);
                    next->distributionValue[V10] = current->distributionValue[V10];
                    // 14
                    next = getCell(x - 1, y, z + 1);
                    next->distributionValue[V14] = current->distributionValue[V14];
                }
                // 15
                next = getCell(x, y + 1, z + 1);
                next->distributionValue[V15] = current->distributionValue[V15];
                // 18
                next = getCell(x, y - 1, z + 1);
                next->distributionValue[V18] = current->distributionValue[V18];
                // 3
                next = getCell(x, y + 1, z);
                next->distributionValue[V3] = current->distributionValue[V3];
                // 5
                next = getCell(x, y, z + 1);
                next->distributionValue[V5] = current->distributionValue[V5];
            }
        }
    }



    //call the redistribution/acceleration in the first slice
    if (myrank == 1) {
        accelerateBW();
    }

    if (myrank == nprocs - 1) {
        accelerateFW();
    }
    // cout << "\t\t\t Propagation..done! Waiting for synch..." << endl;
    MPI::COMM_WORLD.Barrier();
    int size = max_y * max_z * sizeof(bufferData);


    for (unsigned int n = 0; n < 2; n++) {
        if (n) {
            // cout << "\t\t\t EVEN Propagation..done! Waiting for synch..." << endl;
            MPI::COMM_WORLD.Barrier();
        }
        //let even ranks send Buffer first and odd receive
        if ((!n && !(myrank & 1)) || (n && (myrank & 1))) {
            MPI::COMM_WORLD.Send(bufferLeft, size, MPI::BYTE, leftNB, MPI_Data_IncRight);
            MPI::COMM_WORLD.Send(bufferRight, size, MPI::BYTE, rightNB, MPI_Data_IncLeft);
        } else {
            int receiving = 2;
            while (receiving) {

                // cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
                //receive commands or data
                MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, status);
                if (status.Get_tag() == MPI_Data_IncRight || status.Get_tag() == MPI_Data_IncLeft) {
                    MPI::COMM_WORLD.Recv(buffer, size, MPI::BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, status);
                    // cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
                    //decide what to do by analysing the status tag
                    switch (status.Get_tag()) {
                    case MPI_Data_IncLeft:
                        for (unsigned int y = 0; y < max_y; y++) {
                            for (unsigned int z = 0; z < max_z; z++) {
                                //fields going to right domain
                                // 1
                                getCell(0, y, z)->distributionValue[V1] = getCell(buffer, y, z)->distributionValue[B1];
                                // 7
                                getCell(0, y, z)->distributionValue[V7] = getCell(buffer, y, z)->distributionValue[B7];
                                // 9
                                getCell(0, y, z)->distributionValue[V9] = getCell(buffer, y, z)->distributionValue[B9];
                                // 11
                                getCell(0, y, z)->distributionValue[V11] = getCell(buffer, y, z)->distributionValue[B11];
                                // 13
                                getCell(0, y, z)->distributionValue[V13] = getCell(buffer, y, z)->distributionValue[B13];

                            }
                        }

                        receiving--;
                        break;

                    case MPI_Data_IncRight:
                        for (unsigned int y = 0; y < max_y; y++) {
                            for (unsigned int z = 0; z < max_z; z++) {
                                //fields going to left domain
                                // 2
                                getCell(max_x - 1, y, z)->distributionValue[V2] = getCell(buffer, y, z)->distributionValue[B2];
                                // 8
                                getCell(max_x - 1, y, z)->distributionValue[V8] = getCell(buffer, y, z)->distributionValue[B8];
                                // 10
                                getCell(max_x - 1, y, z)->distributionValue[V10] = getCell(buffer, y, z)->distributionValue[B10];
                                // 12
                                getCell(max_x - 1, y, z)->distributionValue[V12] = getCell(buffer, y, z)->distributionValue[B12];
                                // 14
                                getCell(max_x - 1, y, z)->distributionValue[V14] = getCell(buffer, y, z)->distributionValue[B14];
                            }
                        }

                        receiving--;
                        break;
                    }
                }
            }
        }
    }

    // cout << "\t\t\t ODD Propagation..done! Waiting for synch..." << endl;
    MPI::COMM_WORLD.Barrier();
}

void MD3Q19b::accelerateBW()
{
    //process acceleration on sim 1 left buffer

    for (unsigned int y = 1; y < max_y - 1; y++) {
        for (unsigned int z = 1; z < max_z - 1; z++) {
            bufferData *c = getBCell(0, y, z);

            if (c->distributionValue[B2] - t1_accel > 0 && c->distributionValue[B8] - t2_accel > 0 && c->distributionValue[B10] - t2_accel > 0 && c->distributionValue[B12] - t2_accel > 0 && c->distributionValue[B14] - t2_accel > 0) {

                c->distributionValue[B2] += (double) -t1_accel;
                c->distributionValue[B8] += (double) -t2_accel;
                c->distributionValue[B10] += (double) -t2_accel;
                c->distributionValue[B12] += (double) -t2_accel;
                c->distributionValue[B14] += (double) -t2_accel;
            }
        }
    }
}
void MD3Q19b::accelerateFW()
{
    //process acceleration on sim 1 left buffer

    for (unsigned int y = 1; y < max_y - 1; y++) {
        for (unsigned int z = 1; z < max_z - 1; z++) {
            bufferData *c = getBCell(1, y, z);
            cellData *c2 = getCell(max_x - 1, y, z);

            if (c2->distributionValue[V2] - t1_accel > 0 && c2->distributionValue[V8] - t2_accel > 0 && c2->distributionValue[V10] - t2_accel > 0 && c2->distributionValue[V12] - t2_accel > 0 && c2->distributionValue[V14] - t2_accel > 0) {


                c->distributionValue[B1] += (double) t1_accel;
                c->distributionValue[B7] += (double) t2_accel;
                c->distributionValue[B9] += (double) t2_accel;
                c->distributionValue[B11] += (double) t2_accel;
                c->distributionValue[B13] += (double) t2_accel;
            }
        }
    }
}

void MD3Q19b::listenForCommand()
{
    running = true;

    while (running) {

        // cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
        //receive commands or data
        MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);

        // cout << "\t\t\t MSG Received...(" << myrank << ") " << status.Get_tag() << endl;
        //decide what to do by analysing the status tag
        switch (status.Get_tag()) {
        case MPI_Disconnect:
            {
                running = false;
                break;
            }
        case MPI_Update_Area:
            {
                MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                waitForUpdatedArea();
                // cout << "\t\t\t Sim " << myrank << " waiting for synch..." << endl;
                MPI::COMM_WORLD.Barrier();
                // cout << "\t Sim " << myrank << " synched!";
                break;
            }
        case MPI_Set_Area:
            {
                MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                waitForArea();
                // cout << "\t\t\t Sim " << myrank << " waiting for synch..." << endl;
                MPI::COMM_WORLD.Barrier();
                // cout << "\t Sim " << myrank << " synched!";
                break;
            }
        case MPI_Update_Enviroment:
            {
                MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                enviroment envi;
                envi.accel = 0.;
                envi.dense = 0.;
                envi.relax = 0.;

                MPI::COMM_WORLD.Recv(&envi, sizeof(enviroment), MPI::BYTE, OVERMIND, MPI_Update_Enviroment, status);
                if ((status.Get_tag() == MPI_Update_Enviroment)
                    && (status.Get_source() == OVERMIND)) {
                    MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);
                }
                tau_inv = stdRelaxationValue = envi.relax;
                stdDensity = envi.dense;
                stdAcceleration = envi.accel;

                //Tau Values for later distribution calculation
                t0 = stdDensity / 3.0;
                t1 = stdDensity / 18.0;
                t2 = stdDensity / 36.0;

                t1_accel = stdAcceleration * stdDensity / 18.;
                t2_accel = stdAcceleration * stdDensity / 36.;

                // cout << "\t\t\t Sim " << myrank << " waiting for synch..." << endl;
                MPI::COMM_WORLD.Barrier();
                // cout << "\t Sim " << myrank << " synched!";
                break;
            }
        case MPI_Model_Propagate:
            {
                // sleep(10);
                // MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                propagate();
                break;
            }
        case MPI_Model_Collision:
            {
                // MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                collision();
                break;
            }
        case MPI_Filter:
            {
                // MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, OVERMIND, MPI_Ack);
                filter();
                break;

            }
        default:
            {
                // cout << "\t\t\t Error receiving Message from: " << status.Get_source() << endl;
                // cout << "\t\t\t Received unknown Tag: " << status.Get_tag() << endl;
                break;
            }
        };
    }
    // cout << "\t\t\t exiting listener!" << endl;
    return;
}

void MD3Q19b::waitForUpdatedArea()
{
    bool receiving = true;

    while (receiving) {
        cellData *currentCell;
        bufferdata data;
        //cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
        //receive commands or data
        MPI::COMM_WORLD.Probe(OVERMIND, MPI_ANY_TAG, status);

        //cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
        //decide what to do by analysing the status tag
        switch (status.Get_tag()) {
        case MPI_Update_Field:
            {
                MPI::COMM_WORLD.Recv(&data, sizeof(bufferdata), MPI::BYTE, OVERMIND, MPI_Update_Field, status);
                currentCell = getCell(data.x, data.y, data.z);
                if (!currentCell->solid) {
                    currentCell->solid = 1;
                } else {
                    currentCell->solid = 0;
                }

                currentCell->density = stdDensity;
                double *values = currentCell->distributionValue;

                values[0] = t0;
                unsigned int i;
                for (i = 1; i <= 6; i++)
                    values[i] = t1;
                for (i = 7; i <= 18; i++)
                    values[i] = t2;

                break;
            }
        case MPI_Update_Field_Done:
            MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, OVERMIND, MPI_Update_Field_Done, status);
            receiving = false;
            break;
        }
    }
}

void MD3Q19b::waitForArea()
{
    bool receiving = true;

    simField field;
    MPI::COMM_WORLD.Recv(&field, sizeof(simField), MPI::BYTE, MPI_ANY_SOURCE, MPI_Set_Area + 1, status);

    // cout << "\t\t\t MSG Received...(" << myrank << ") " << status.Get_tag() << endl;

    max_x = field.dim_x;
    max_y = field.dim_y;
    max_z = field.dim_z;
    if (cellDataModel)
        delete[]cellDataModel;
    cellDataModel = new cellData[max_x * max_y * max_z];
    for (unsigned int i = 0; i < max_x * max_y * max_z; i++) {
        initCell(&cellDataModel[i]);
    }

    while (receiving) {
        bufferdata data;
        //cout << "\t\t\t Sim " << myrank << " is Listening" << endl;
        //receive commands or data
        MPI::COMM_WORLD.Probe(OVERMIND, MPI_ANY_TAG, status);

        //cout << "\t\t\t MSG Received..." << status.Get_tag() << endl;
        //decide what to do by analysing the status tag
        switch (status.Get_tag()) {
        case MPI_Field:
            MPI::COMM_WORLD.Recv(&data, sizeof(bufferdata), MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);
            getCell(data.x, data.y, data.z)->solid = 1;
            break;

        case MPI_Field_Done:
            MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, OVERMIND, MPI_ANY_TAG, status);
            receiving = false;
            break;
        }
    }

    cellData *currentCell;
    for (unsigned int x = 0; x < max_x; x++) {
        for (unsigned int y = 0; y < max_y; y++) {
            for (unsigned int z = 0; z < max_z; z++) {
                currentCell = getCell(x, y, z);
                if ((z < 1 || z > max_z - 2 || y < 1 || y > max_y - 2))
                    currentCell->solid = true;

                currentCell->density = stdDensity;
                double *values = currentCell->distributionValue;

                values[0] = t0;
                unsigned int i;
                for (i = 1; i <= 6; i++)
                    values[i] = t1;
                for (i = 7; i <= 18; i++)
                    values[i] = t2;
            }
        }
    }

    allocateBuffers();

//      MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, MPI_Ack);
    cout << "\t\t\t Area Set: Dimension, " << max_x << ":" << max_y << ":" << max_z << endl;
}

void MD3Q19b::allocateBuffers()
{
    if (bufferLeft)
        delete[]bufferLeft;
    if (bufferRight)
        delete[]bufferRight;
    if (buffer)
        delete[]buffer;
    bufferLeft = new bufferData[max_y * max_z];
    bufferRight = new bufferData[max_y * max_z];
    buffer = new bufferData[max_y * max_z];

    for (unsigned int i = 0; i < max_y * max_z; i++) {
        initBCell(&bufferLeft[i]);
        initBCell(&bufferRight[i]);
        initBCell(&buffer[i]);
    }
}

void MD3Q19b::initBCell(bufferData * cell)
{
    double *values = cell->distributionValue;

    for (unsigned int i = 0; i < 5; i++)
        values[i] = 0.0;
}

void MD3Q19b::initCell(cellData * cell)
{
    cell->solid = false;
    cell->density = 0.0;
    double *values = cell->distributionValue;

    for (unsigned int i = 0; i <= 18; i++)
        values[i] = 0.0;
}
