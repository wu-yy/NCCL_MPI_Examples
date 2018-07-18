# include<stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>   //gethostname()
# include <stdint.h>   //uint64_t
# include "mpi.h"

static void getHostName(char* hostname, int maxlen) {
  gethostname(hostname, maxlen);
  for (int i=0; i< maxlen; i++) {
    if (hostname[i] == '.') {
        hostname[i] = '\0';
        return;
    }
  }
}
static uint64_t getHostHash(const char * string){
	// Based on DJB2, result = result * 33 + char
  uint64_t result = 5381;
  for (int c = 0; string[c] != '\0'; c++){
    result = ((result << 5) + result) + string[c];
  }
  return result;
}
int main(int argc,char**argv){

	int myid,numprocs,source=0;
	MPI_Status status;
	int localRank = 0;

	char message[100];
	MPI_Init(&argc,&argv);
	
	//返回通信线程的id
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

	//返回通信子的进程总数
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);


	char hostname[1024];
	getHostName(hostname, 1024);
	printf("hostnae is %s\n",hostname);

	uint64_t hostHashs[100];
	hostHashs[myid] = getHostHash(hostname);

	//对取得的hostHash做Allgather
	MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, hostHashs, sizeof(uint64_t), MPI_BYTE, MPI_COMM_WORLD);
 
	for (int p=0; p<numprocs; p++) {
     if (p == myid) break;
	 printf("begin localRank++\n");
	 printf("p hashs:%lld; myhashs:%lld",hostHashs[p],hostHashs[myid]);
	 if (hostHashs[p] == hostHashs[myid]) {
		 localRank++;
		}
	}
	printf("localRank is %d\n",localRank);

	if(myid!=0){
		
		strcpy(message,"Hello world! kk"); //为发送的字符串赋值
		MPI_Send(message,strlen(message)+1,MPI_CHAR,0,99,MPI_COMM_WORLD);//发送字符串时长度要加1，从而包括串结束标志
	}else{	
		//除0进程的其他进程接收来自于0进程的字符串数据
		for(source = 1;source < numprocs;source++)
		{
			MPI_Recv(message,100,MPI_CHAR,source,99,MPI_COMM_WORLD,&status);
			printf("I am process %d.I receive string '%s' from process %d.\n",myid,message,source);
		}
	}
	MPI_Finalize();

	for(source = 1;source < numprocs;source++)
	{
		printf("END: I am process %d.I receive string '%s' from process %d.\n",myid,message,source);
	}
	return 0;

}
