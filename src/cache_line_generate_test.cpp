
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
uint64_t start_address = 0x55b9f242e850;
uint64_t cache_line_size = 64;


uint64_t returnLine(uint64_t address)
{
	return ((address/cache_line_size) - (start_address/cache_line_size));
}

int main()
{

	int num_nodes = 32768;

	// for(uint64_t i=0;i<num_nodes;i++)
	// {
	// 	cout<< returnLine(start_address + (i*8))<<" ";
	// }

	cout<< sizeof(double);
	// int arr[] = {1,2,3};
	// int *p = arr;
	// cout<<"\t"<<p;
	// p++;
	// cout<<"\t"<<p;

	return 0;
}