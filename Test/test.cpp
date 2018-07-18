#include<map>
#include<string>
#include<chrono>
#include<thread>
#include<mutex>
#include<iostream>
#include<malloc.h>
using namespace std;

int main(){

	int n=4;
	int m=5;
	int** subject=(int**)malloc(n*sizeof(int*));

	for(int i=0;i<n;i++){
		subject[i]=(int*)malloc(m*sizeof(int));
	}

	subject[0][0]=1;
	printf("%d",subject[0][0]);
  
	return 0;
}