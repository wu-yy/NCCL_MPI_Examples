//MPI 跟进程从所有的进程获取相同数目的数据
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<vector>
#include<malloc.h>
#include <string>
#include<iostream>
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

	 
	 //MPI_Allgatherv(localData, count[comRank], MPI_INT, globalData, count, disp, MPI_INT, MPI_COMM_WORLD);   // 聚集数据

	 MPI_Gatherv(nullptr, 0, MPI_INT, globalData, count, disp, MPI_INT,0, MPI_COMM_WORLD);
	 
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
	for(i=0;i<N;i++){
		send_buffer[i]=2*(i+1);
	}
	for (i = 0; i < nprocs*N; i++)
		//recv_buffer[i+myrank*N] = myrank + i;
			recv_buffer[i]=i;
	// 指定进程0为根进程.
	root = 0;
	int encoded_message_length=80;
	if (myrank == root)
	{
		for (i = 0; i < nprocs * N; i++)
			printf("before gathering, recv_buffer[%d] = %d\n", i, recv_buffer[i]);
			printf("\n");
			encoded_message_length=199;
	}
	
	//MPI_Gather(send_buffer, N, MPI_INT,recv_buffer, N, MPI_INT, root, MPI_COMM_WORLD);
	//if (myrank == root)
	//MPI_Gather(MPI_IN_PLACE, N, MPI_INT, recv_buffer, N, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Gather(&encoded_message_length, 1, MPI_INT, nullptr, 1, MPI_INT,
			   0, MPI_COMM_WORLD);
	//MPI_Allgather(send_buffer, N, MPI_INT,recv_buffer, N, MPI_INT, MPI_COMM_WORLD);

	if (myrank == root)
	{
		for (i = 0; i < nprocs * N; i++)
			printf("process:%d,after gathering, recv_buffer[%d] = %d\n",myrank, i, recv_buffer[i]);
			free(recv_buffer);
	}
	printf("rank is %d,encoded_message_length:%d\n",myrank,encoded_message_length);
	// delete[]send_buffer;
	// delete[]recv_buffer;
	MPI_Finalize();
}

void testMPI_Gather_Asy(int argc,char **argv){
	int N=1;
	int myrank, nprocs, root;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	
	// 指定进程0为根进程.
	root = 0;
	int encoded_message_length=80;
	if (myrank == root)
	{
		/*
		for (i = 0; i < nprocs * N; i++)
			printf("before gathering, recv_buffer[%d] = %d\n", i, recv_buffer[i]);
			printf("\n");
			encoded_message_length=199;*/

		// 1. Get message lengths from every rank.
		auto recvcounts = new int[nprocs];
		recvcounts[0] = 0;
		MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, recvcounts, 1, MPI_INT, root,
				   MPI_COMM_WORLD);


		// 2. Compute displacements.
		auto displcmnts = new int[nprocs];
		size_t total_size = 0;
		for (int i = 0; i < nprocs; i++) {
		  if (i == 0) {
			displcmnts[i] = 0;
		  } else {
			displcmnts[i] = recvcounts[i - 1] + displcmnts[i - 1];
		  }
		  total_size += recvcounts[i];
		}

		// 3. Collect messages from every rank.
		auto buffer = new char[total_size];
		MPI_Gatherv(nullptr, 0, MPI_BYTE, buffer, recvcounts, displcmnts, MPI_BYTE,
					root, MPI_COMM_WORLD);
	}else{

		std::string encoded_message="children"+std::to_string(myrank);
		 int encoded_message_length = (int)encoded_message.length() + 1;

		//在这里循环不停的进行MPI的通信
		MPI_Gather(&encoded_message_length, 1, MPI_INT, nullptr, 1, MPI_INT,
				   root, MPI_COMM_WORLD);

		MPI_Gatherv((void*)encoded_message.c_str(), encoded_message_length,
					MPI_BYTE, nullptr, nullptr, nullptr, MPI_BYTE, root,
					MPI_COMM_WORLD);

	}
	
	// delete[]send_buffer;
	// delete[]recv_buffer;
	MPI_Finalize();
}

int main(int argc, char *argv[])
{
	//TestScatterv(argc,argv);
	//TestAllGatherrv(argc,argv);
	//testAllGather(argc,argv);
	//TestAllGatherrv(argc,argv);
	//testMPI_Gather(argc,argv);
	testMPI_Gather_Asy(argc,argv);
	return 0;


}