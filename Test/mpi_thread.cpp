#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<malloc.h>

int main(int *argc,char *argv[]){
	int provided;
	MPI_Init_thread(&argc,&argv,MPI_THREAD_MULTIPLE, &provided);  
	if(provided!=MPI_THREAD_MULTIPLE){
		printf("MPI do not Support  Multiple thread!\n");
		exit(0);

	}

	return 0;
}

