# 编译运行 mpi_test
编译：
mpicc -o3 mpi_test.c -o mpi_test
执行：
mpirun -n 4 mpi_test


# 编译执行 singleProcess.cpp
nvcc -o first_cuda singleProcess.cpp  -I/usr/local/nccl/include -L/usr/local/nccl/lib -l nccl


# 使用编译mpi 程序
mpicc -o3 oneDevicePerprocess.cpp -o cpi -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart -lcuda  -I/usr/local/nccl/include -L/usr/local/nccl/lib -l nccl


#使用 run.sh 编译运行文件
./run.sh [source.cpp]  output