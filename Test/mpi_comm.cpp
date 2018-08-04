#include "mpi.h"
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#define nodeNameLength 20
#define LEN 5
int main(int argc, char  *argv[])
{
	//MPI 组间通信
   MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_size);

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int n = 3;
    const int ranks[3] = {0,2,4};
    const int ori1[1] = {0};
    const int ori2[1] = {1};
    int root1, root2;

    // 从world_group进程组中构造出来两个进程组
    MPI_Group group1, group2;
    MPI_Group_incl(world_group, n, ranks, &group1);
    MPI_Group_excl(world_group, n, ranks, &group2);
    // 根据group1 group2分别构造两个通信域
    MPI_Comm comm1, comm2;
    MPI_Comm_create(MPI_COMM_WORLD, group1, &comm1);
    MPI_Comm_create(MPI_COMM_WORLD, group2, &comm2);

    // 维护发送缓冲区和接受缓冲区
    int i;
    double *sbuf, *rbuf1, *rbuf2, *rbuf3;
	sbuf = (double*)malloc(LEN*sizeof(double));
	rbuf1 = (double*)malloc(LEN*sizeof(double));
	rbuf2 = (double*)malloc(LEN*sizeof(double));
	rbuf3 = (double*)malloc(LEN*sizeof(double));
    srand(world_rank*100);
    for(i=0; i<LEN; i++) sbuf[i] = i; // RAND_MAX;
    fprintf(stderr,"rank %d:\t sendBuf is:", world_rank);
    for(i=0; i<LEN; i++) fprintf(stderr,"%f\t",sbuf[i]);
    fprintf(stderr,"\n");

	//将一个组中的进程标识号转换成另一个组的进程标识号
    MPI_Group_translate_ranks(world_group, 1, ori1, group1, &root1);
    MPI_Group_translate_ranks(world_group, 1, ori2, group2, &root2);
    // MPI_COMM_WORLD comm1 comm2分别执行不同的归约操作
    if (MPI_COMM_NULL!=comm1) { // comm1
        MPI_Reduce(sbuf, rbuf1, LEN, MPI_DOUBLE, MPI_MAX, root1, comm1);
        int rank_1;
        MPI_Comm_rank(comm1, &rank_1);
        if (root1==rank_1) {
			fprintf(stderr,"root1:%d,MAX:\t\n",root1);
            for(i=0; i<LEN; i++) fprintf(stderr,"%f\t",rbuf1[i]);
            fprintf(stderr,"\n");
        }
    } 
    else if (MPI_COMM_NULL!=comm2) { // comm2
        MPI_Reduce(sbuf, rbuf2, LEN, MPI_DOUBLE, MPI_MIN, root2, comm2);
        int rank_2;
        MPI_Comm_rank(comm2, &rank_2);
        if (root2==rank_2) {
			fprintf(stderr,"root2:%d,MIN:\t\n",root2);
            for(i=0; i<LEN; i++) fprintf(stderr,"%f\t",rbuf2[i]);
            fprintf(stderr,"\n");
        }
    }
    MPI_Reduce(sbuf, rbuf3, LEN, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // MPI_COMM_WORLD 
    if (0==world_rank) {
        fprintf(stderr,"SUM:\t\n");
        for(i=0; i<LEN; i++) fprintf(stderr,"%f\t",rbuf3[i]);
        fprintf(stderr,"\n");
    }
    // 清理进程组和通信域
    if(MPI_GROUP_NULL!=group1) MPI_Group_free(&group1);
    if(MPI_GROUP_NULL!=group2) MPI_Group_free(&group2);
    if(MPI_COMM_NULL!=comm1) MPI_Comm_free(&comm1);
    if(MPI_COMM_NULL!=comm2) MPI_Comm_free(&comm2);
    MPI_Finalize();
}
