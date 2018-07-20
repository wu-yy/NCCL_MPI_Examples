//MPI 跟进程从所有的进程获取相同数目的数据
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<malloc.h>
//MPI_Scatterv

void TestScatterv(int argc,char**argv){
	int rank, size;     // name of process, number of process
    int *sendcounts;    // array indicating how many elements will be sent from root
    int *displs;        // array indicating the displacements where each segment begins
	int N=4;
	int Array[N];  // data need to be sent
    for(int i=0;i<N;i++)Array[i] = i;  //initialize data

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sendcounts = (int*)malloc(sizeof(int)*size);
    displs = (int*)malloc(sizeof(int)*size);

    int sum = 0;            // Sum of distributed elements, can be used to calculate displacements for each segment
    int rec_buf[100];    // buffer where the received data should be saved for each process

    // Step 1: calculate send counts and displacements
    for (int i = 0; i < size; i++) {
        sendcounts[i] = N/size;
        if(i==0)sendcounts[i] += N%size;
        displs[i] = sum;
        sum += sendcounts[i];
    }

    // output sendcounts and displacement for each process at root rank
    if (0 == rank) {
        for (int i = 0; i < size; i++) {
			printf("Rank: %d, sendcounts:%d,displacement:%d\n",i,sendcounts[i],displs[i]);
          }
    }

    // scatter the data among processes
    MPI_Scatterv(&Array, sendcounts, displs, MPI_INT, &rec_buf, 100, MPI_INT, 0, MPI_COMM_WORLD);

    // output received data for each process
	printf("rank=%d ,REceived: ",rank);
	for (int i = 0; i < sendcounts[rank]; i++) {
			printf("%d\t",rec_buf[i]);
	}
    printf("\n");

    MPI_Finalize();

    free(sendcounts);
    free(displs);

}


//MPI_Allgatherv  不同长度的数据块收集，等价于依次以Comm中的每个进程为根进程调用MPI_Gatherv 或是以任一进程为跟进程
//调用一次普通收集，紧接着再对收集到的数据进行一次广播

void TestAllGatherrv(int argc,char**argv){
	 const int localSize=100,nProcess=8,globalSize=100;
	 int globalData[globalSize],localData[localSize],count[nProcess],disp[nProcess];
	 int comRank,comSize,i;

	 MPI_Init(&argc,&argv);
	 MPI_Comm_rank(MPI_COMM_WORLD,&comRank);
	 MPI_Comm_size(MPI_COMM_WORLD,&comSize);
	 if(comRank==0)
		 for(i=0;i<globalSize;globalData[i]=i,i++);
	 else
		for(i=0;i<globalSize;globalData[i]=0,i++);

	 for(i=0;i<localSize;localData[i++]=-1);
	 for(i=0;i<comSize;count[i]=i+1,i++);
	 for (disp[0] = 0, i = 1; i < comSize; disp[i] = disp[i - 1] + count[i - 1], i++);

	 MPI_Scatterv(globalData, count, disp, MPI_INT, localData, count[comRank], MPI_INT, 0, MPI_COMM_WORLD);  // 分发数据

	 printf("rank=%d ,REceived: ",comRank);
	 for (int i = 0; i < count[comRank]; i++) {
		 printf("%d\t",localData[i]);
	}
    printf("\n");

	
	 for (i = 0; i < count[comRank]; i++)
	   localData[i] += comRank;
	
	 printf("rank=%d ,REceived+ID: ",comRank);
	 for (int i = 0; i < count[comRank]; i++) {
		 printf("%d\t",localData[i]);
	}
    printf("\n");

	 
	 MPI_Allgatherv(localData, count[comRank], MPI_INT, globalData, count, disp, MPI_INT, MPI_COMM_WORLD);   // 聚集数据
	  printf("globalData:\n");
	 for (i = 0; i < globalSize; i++)
		 printf("%d ", globalData[i]);

	 printf("\n");

	MPI_Finalize();
}

void testMPI_Gather(int argc,char **argv){
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
}

int main(int argc, char *argv[])
{
	//TestScatterv(argc,argv);
	TestAllGatherrv(argc,argv);
	return 0;


}