#!/bin/sh ÄÚÖÃÊ¹ÓÃg++ ±àÒëÆ÷

sourcefile=$1
outputfile=$2
mpic++ -o3 ${sourcefile} -o ${outputfile} -std=c++11 -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart -lcuda  -I/usr/local/nccl/include -L/usr/local/nccl/lib -l nccl

