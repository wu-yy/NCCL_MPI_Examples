//MPI 跟进程从所有的进程获取相同数目的数据
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<malloc.h>

int main(int argc, char *argv[])
{
	int N=1;
	int i, myrank, nprocs, root;
    int *send_buffer;
    int *recv_buffer;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	send_buffer = (int*)malloc(N*sizeof(int));//new int[N];
	recv_buffer = (int*)malloc(nprocs*N*sizeof(int));
    for (i = 0; i < N; i++)
        send_buffer[i] = myrank + i;

	// 指定进程0为根进程.
    root = 0;
    if (myrank == root)
    {
        for (i = 0; i < nprocs * N; i++)
            printf("before gathering, recv_buffer[%d] = %d\n", i, recv_buffer[i]);
            printf("\n");
    }
	
	//MPI_Gather(send_buffer, N, MPI_INT,recv_buffer, N, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Allgather(send_buffer, N, MPI_INT,recv_buffer, N, MPI_INT, MPI_COMM_WORLD);

	//if (myrank == root)
    {
        for (i = 0; i < nprocs * N; i++)
			printf("process:%d,after gathering, recv_buffer[%d] = %d\n",myrank, i, recv_buffer[i]);
        free(recv_buffer);
    }
    // delete[]send_buffer;
    // delete[]recv_buffer;
    MPI_Finalize();
    return 0;


}