// 快速排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "vector"
#include "fstream"

using namespace std;

void fastSort(int, int, vector<int>&);
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
	fastSort(0, nums.size() - 1, nums);
	for (int num : nums) {
		cout << num << ends;
	}

    return 0;
}

void fastSort(int start, int end, vector<int>&nums) {
	if (start >= end) {
		return;
	}

	int pivot = nums[start];
	int left = start, right = end;
	while (left < right) {
		//因为pivot已经把中间数字记录下来了，所以可以将后边的数字移动下来，填补空缺
		while (left < right&&nums[right] >= pivot) {
			right--;
		}
		nums[left] = nums[right];

		//因为后边的数字已经移动下来了，原先位置已经空了，可以从前端移动一个到后端补空缺
		while (left < right&&nums[left] <= pivot) {
			left++;
		}
		nums[right] = nums[left];
	}

	nums[left] = pivot;
	fastSort(start, left - 1, nums);
	fastSort(left + 1, end, nums);
}

