#include<mpi.h>                                                                                                                         1,1           All
#include <cuda_runtime.h>
#include <nccl.h>
#define HAVE_CUDA
#define CPU_DEVICE_ID -1
using namespace std;
#include<iostream>

ncclResult_t ncclCommInitAll(ncclComm_t* comm, int ndev, const int* devlist) {
  ncclUniqueId Id;
  ncclGetUniqueId(&Id);
  ncclGroupStart();
  for (int i=0; i<ndev; i++) {
    cudaSetDevice(devlist[i]);
    ncclCommInitRank(comm+i, ndev, Id, i);
  }
  ncclGroupEnd();
}
int main(){



}