#ifndef _MPITAGS_H_
#define _MPITAGS_H_

#define OVERMIND 0

//Defining the custom MPI Tags here for datacommunications.
#define MPI_Ack					100
#define MPI_Disconnect			101
#define MPI_Sim_Halted			111

#define MPI_Data_IncLeft		301
#define MPI_Data_IncRight		302
#define MPI_Prop_Done			303

#define MPI_Data_OutRight		311
#define MPI_Data_OutLeft		312

#define MPI_Model_Propagate 	500
#define MPI_Model_Collision		600

#define MPI_Update_Enviroment	700

#define MPI_Set_Area			800
#define MPI_Field				805
#define MPI_Field_Done			806

#define MPI_Filter			900
#define MPI_Get_Cells			901
#define MPI_Set_Req_Size		902
#define MPI_Set_Resp_Size		903
#define MPI_Filter_Done			904
#define MPI_Send_MinMax			905
#define MPI_Get_Cell			906

#define MPI_Update_Area			850
#define MPI_Update_Field		855
#define MPI_Update_Field_Done		856



#endif //_MPITAGS_H_
