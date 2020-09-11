// 归并排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;
void mergeSort(int, int, vector<int>&);
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	int size = 0;
	input >> size;
	vector<int> nums(size);
	for (int i = 0; i < size; i++) {
		input >> nums[i];
	}

	mergeSort(0, size - 1, nums);
	for (int num : nums) {
		cout << num << ends;
	}
    return 0;
}

void mergeSort(int start,int end,vector<int>&nums) {
	if (start >= end) {
		return;
	}

	int mid = start + (end - start) / 2;
	mergeSort(start, mid, nums);
	mergeSort(mid + 1, end, nums);
		
	vector<int> temp;
	int i = start, j = mid + 1;	
	while (i <= mid&&j <= end) {
		if (nums[i] <= nums[j]) {
			temp.push_back(nums[i++]);
		}
		else {
			temp.push_back(nums[j++]);
		}
	}
	while (j <= end) {
		temp.push_back(nums[j++]);
	}
	while (i <= mid) {
		temp.push_back(nums[i++]);
	}

	for (int i = 0, j = start; j <= end; j++, i++) {
		nums[j] = temp[i];
	}
}

