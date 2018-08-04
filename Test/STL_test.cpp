#include<iostream>
#include<unordered_map>
using namespace std;
int main(){

	unordered_map<int,tuple<int,int>> A;
	A.insert(1,(2,2));
	for(auto i:A){
		cout<<i.first<<endl;
	}
	return 0;
}