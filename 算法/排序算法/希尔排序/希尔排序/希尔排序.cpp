// 希尔排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;

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

	//每次排序间隔interval个元素的元素
	for (int interval = size/2; interval > 0; interval /= 2) {
		//从start开始，每隔interval个元素进行排序，采用的选择排序
		for (int start = interval; start < size; start++) {
			int index=start,val = nums[start];
			//寻找插入位置，同时将元素后移
			while (index - interval >= 0 && val < nums[index - interval]) {
				nums[index] = nums[index - interval];
				index -= interval;
			}
			nums[index] = val;
		}
	}

	for (int num : nums) {
		cout << num << ends;
	}
	cout << endl;
    return 0;
}

