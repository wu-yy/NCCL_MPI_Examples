#include "mpi.h"
#include <stdio.h>
#include <math.h>

int main(int argc,char**argv){

	int myid, numprocs;
	int namelen;
 
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	for(int i=0;i<argc;i++)
		printf("argc: %d;argv:%s\n",i,argv[i]);	
 
	MPI_Init(&argc, &argv);

	////获得进程号
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	//返回通信子的进程总数
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	MPI_Get_processor_name(processor_name, &namelen);
 
	fprintf(stderr, "Hello World! Process %d of %d on %s\n", myid, numprocs, processor_name);
	fprintf(stderr, "Process namelen %d\n", namelen);


	MPI_Finalize();
	return 0;
}
