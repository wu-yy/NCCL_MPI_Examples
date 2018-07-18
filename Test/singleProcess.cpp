#include<stdio.h>
#include <malloc.h>
#include<stdlib.h>
#include "cuda_runtime.h"
#include "nccl.h"



#define CUDACHECK(cmd) do {                         \
  cudaError_t e = cmd;                              \
  if( e != cudaSuccess ) {                          \
    printf("Failed: Cuda error %s:%d '%s'\n",             \
        __FILE__,__LINE__,cudaGetErrorString(e));   \
    exit(1);                             \
  }                                                 \
} while(0)

			
#define NCCLCHECK(cmd) do{   \
	ncclResult_t r=cmd;    \
	if(r!=ncclSuccess){     \
		printf("Failed, NCCL error %s:%d '%s'\n",__FILE__,__LINE__,ncclGetErrorString(r));   \
		exit(1);    \
	}                        \
} while(0)

int main(int argc,char**argv){
	printf("begin run main...\n");

	ncclComm_t comms[4];

	//manageing 4 devices
	int nDev=4;
	int size=32*1024*1024;
	int devs[4]={0,1,2,3};

	//allocating and initializing device buffers
	float ** sendbuff=(float**)malloc(nDev*sizeof(float*));
	float ** recvbuff=(float**)malloc(nDev*sizeof(float*));

	cudaStream_t *s=(cudaStream_t*)malloc(nDev*sizeof(cudaStream_t));

	for(int i=0;i<nDev;i++){
		printf("cudaSetdevice..\n");
		CUDACHECK(cudaSetDevice(i));
		CUDACHECK(cudaMalloc(sendbuff+i,size*sizeof(float)));
		CUDACHECK(cudaMalloc(recvbuff+i,size*sizeof(float)));
		CUDACHECK(cudaMemset(sendbuff[i],1,size*sizeof(float)));
		CUDACHECK(cudaMemset(recvbuff[i],0,size*sizeof(float)));
	}
	//initilzing NCCL
	NCCLCHECK(ncclCommInitAll(comms,nDev,devs));
	//calling NCCL communication API. Group API is required when using
	//multiple devices per thread
	NCCLCHECK(ncclGroupStart());

	for (int i = 0; i < nDev; ++i)
		NCCLCHECK(ncclAllReduce((const void*)sendbuff[i], (void*)recvbuff[i], size, ncclFloat, ncclSum,
        comms[i], s[i]));
	
	NCCLCHECK(ncclGroupEnd());

  //synchronizing on CUDA streams to wait for completion of NCCL operation
  for (int i = 0; i < nDev; ++i) {
    CUDACHECK(cudaSetDevice(i));
    CUDACHECK(cudaStreamSynchronize(s[i]));
  }

  //free device buffers
  for (int i = 0; i < nDev; ++i) {
    CUDACHECK(cudaSetDevice(i));
    CUDACHECK(cudaFree(sendbuff[i]));
    CUDACHECK(cudaFree(recvbuff[i]));
  }

  //finalizing NCCL
  for(int i = 0; i < nDev; ++i)
      ncclCommDestroy(comms[i]);

  printf("Success \n");

	return 0;

}


