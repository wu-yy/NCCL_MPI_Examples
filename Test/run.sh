#!/bin/sh

sourcefile=$1
outputfile=$2
mpicc -o3 ${sourcefile} -o ${outputfile} -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart -lcuda  -I/usr/local/nccl/include -L/usr/local/nccl/lib -l nccl

