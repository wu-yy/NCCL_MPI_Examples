#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <cstring>
#include<cstdio>
using namespace std;

int main(){
	//定义需要的设备的属性
	cudaDeviceProp devicePropDefined;
	memset(&devicePropDefined,0,sizeof(cudaDeviceProp));
	devicePropDefined.major=5;  //设备的计算功能集（Compute Capability）的版本号
	devicePropDefined.minor=2;

	int devicedChoosed; //选中的设备的ID
	cudaError_t cudaError;
	cudaGetDevice(&devicedChoosed); //获取当前设备的ID

	cout << "当前使用设备的编号： " << devicedChoosed << endl;
 
	cudaChooseDevice(&devicedChoosed, &devicePropDefined);  //查找符合要求的设备ID
	cout << "满足指定属性要求的设备的编号： " << devicedChoosed << endl;
	cudaError = cudaSetDevice(devicedChoosed); //设置选中的设备为下文的运行设备
 
	if (cudaError == cudaSuccess)
		cout << "设备选取成功！" << endl;
	else
		cout << "设备选取失败！" << endl;
	getchar();
	return 0;

}