// 堆排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;

void build(int index, int len, vector<int>&nums);
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	int size = 0;
	input >> size;
	vector<int> nums(size, 0);
	for (int i = 0; i < size; i++) {
		input >> nums[i];
	}

	//初始建立一个大顶堆
	for (int i = size / 2; i >= 0; i--) {
		build(i, size, nums);
	}
	//每次把大顶堆中最大的元素放到最后边一个元素中，作为排好序的元素
	for (int i = size - 1; i > 0; i--) {
		swap(nums[i], nums[0]);
		build(0, i, nums);
	}

	for (int num : nums) {
		cout << num << ends;
	}
	cout << endl;
    return 0;
}

void build(int parent,int len,vector<int>&nums) {
	int maxInd = parent;
	int left = 2 * parent + 1, right = 2 * parent + 2;
	//寻找左右两个子节点中比较大的那一个和父节点进行比较，大者和父节点进行比较，交换。
	if (left < len) {
		maxInd = nums[maxInd] > nums[left] ? maxInd : left;
	}
	if (right < len) {
		maxInd = nums[maxInd] > nums[right] ? maxInd : right;
	}
	

	if (maxInd != parent) {
		swap(nums[parent], nums[maxInd]);
		build(maxInd, len, nums);
	}
}

