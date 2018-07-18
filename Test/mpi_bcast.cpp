
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<malloc.h>
// 数据移动：数据广播

void Bcast(int argc,char*argv[]){
	int myrank, nprocs;
	MPI_Init(&argc,&argv);
	MPI_Barrier(MPI_COMM_WORLD);
	double elapsed_time=-MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

	int sendRecvBuf[3]={0,0,0};

	if(!myrank){
		sendRecvBuf[0]=3;
		sendRecvBuf[1]=6;
		sendRecvBuf[2]=9;
	}
	int count=3;
	int root=0;

	MPI_Bcast(sendRecvBuf,count,MPI_INT,root,MPI_COMM_WORLD);
	printf("my rankID = %d, sendRecvBuf = {%d, %d, %d}\n", myrank, sendRecvBuf[0], sendRecvBuf[1], sendRecvBuf[2]);
	elapsed_time += MPI_Wtime();
	if (!myrank) {
		printf("process :%d,total elapsed time = %10.6f\n",myrank, elapsed_time);
    }

    MPI_Finalize();
}


//数据数据移动-散射
void  Scatter(int argc,char *argv[]){
	int myrank, nprocs;
	int SIZE=4;
	float sendBuf[SIZE][SIZE]={
		{ 1.0,   2.0,    3.0,    4.0 },
        { 5.0,   6.0,    7.0,    8.0 },
        { 9.0,   10.0,   11.0,   12.0 },
        { 13.0,  14.0,   15.0,   16.0 }
	};

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	if (nprocs == SIZE) {
        int source = 0;
        int sendCount = SIZE;
        int recvCount = SIZE;
        float recvBuf[SIZE];
        //scatter data from source process to all processes in MPI_COMM_WORLD  
        MPI_Scatter(sendBuf, sendCount, MPI_FLOAT,
            recvBuf, recvCount, MPI_FLOAT, source, MPI_COMM_WORLD);

        printf("my rankID = %d, receive Results: %f %f %f %f, total = %f\n",
            myrank, recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3],
            recvBuf[0] + recvBuf[1] + recvBuf[2] + recvBuf[3]);
	}else if (nprocs == 8) {
        int source = 0;
        int sendCount = 2;
        int recvCount = 2;
        float recvBuf[2];

        MPI_Scatter(sendBuf, sendCount, MPI_FLOAT,
            recvBuf, recvCount, MPI_FLOAT, source, MPI_COMM_WORLD);

        printf("my rankID = %d, receive result: %f %f, total = %f\n",
			myrank, recvBuf[0], recvBuf[1], recvBuf[0] + recvBuf[1]);
    }

    else {
        printf("Please specify -n %d or -n %d\n", SIZE, 2 * SIZE);
    }

	MPI_Finalize();
}


int main(int argc,char*argv[]){

	Scatter(argc,argv);
	return 0;
}
