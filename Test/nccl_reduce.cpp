//测试NCCL的reduce
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

	ncclComm_t comms[1];

	//manageing 1 devices
	int nDev=4;
	int size=32;
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
		float *h_arr;
		h_arr = (float *)malloc(size*sizeof(float));
		for (int i=0; i<size; ++i)
			h_arr[i] = i; // Or other values
		CUDACHECK(cudaMemcpy(sendbuff[i], h_arr, size*sizeof(float), cudaMemcpyHostToDevice)); //将数据从CPU传递到GPU
		//CUDACHECK(cudaStreamCreate(s+i));
	}
	//initilzing NCCL
	NCCLCHECK(ncclCommInitAll(comms,nDev,devs));
	//calling NCCL communication API. Group API is required when using
	//multiple devices per thread
	NCCLCHECK(ncclGroupStart());

	for (int i = 0; i < nDev; ++i)
		//NCCLCHECK(ncclAllReduce((const void*)sendbuff[i], (void*)recvbuff[i], size, ncclFloat, ncclSum,comms[i], s[i]));
		NCCLCHECK(ncclReduce((const void *)sendbuff[i], (void*)recvbuff[i],size,ncclFloat, ncclSum, 0,comms[i], s[i]));

	NCCLCHECK(ncclGroupEnd());

  //synchronizing on CUDA streams to wait for completion of NCCL operation
  for (int i = 0; i < nDev; ++i) {
	CUDACHECK(cudaSetDevice(i));
	CUDACHECK(cudaStreamSynchronize(s[i]));
  }
  for (int i = 0; i < nDev; ++i) {
       CUDACHECK(cudaSetDevice(i));
       float* recvCPU=(float*)malloc(size*sizeof(float));  //将数据从cuda 拷贝到cpu
       CUDACHECK(cudaMemcpy(recvCPU, recvbuff[i], sizeof(float) * size, cudaMemcpyDeviceToHost));
      printf("End Reduce Dev is %d of process, RecvBUf is %f,%f,%f,%f\n",i,recvCPU[0],recvCPU[1],recvCPU[2],recvCPU[3]);

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


