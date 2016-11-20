#include <vector>

#include <iostream>
#include <signal.h>

using namespace std;

#include "MD3Q19b.h"
#include "types.h"
#include "mpitags.h"


void spawner(int argc, char* argv[])
{
	MD3Q19b gridSpawn;	
}

int main(int argc, char* argv[])
{
    	// might fail on linux
    	signal(SIGCHLD,SIG_IGN);

    	signal(SIGPIPE,SIG_IGN);
    	signal(SIGHUP ,SIG_IGN);
    	signal(SIGALRM,SIG_IGN);
    	signal(SIGINT,SIG_IGN);
    	signal(SIGTERM,SIG_IGN);

	MPI::Init_thread(MPI_THREAD_MULTIPLE);
	// MPI::Init(argc, argv);
/*	int myrank = MPI::COMM_WORLD.Get_rank();
	int nprocs = MPI::COMM_WORLD.Get_size();*/

	spawner(argc, argv);
		
//	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();	
	return 0;	
}

