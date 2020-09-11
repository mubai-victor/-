// 插入排序.cpp : 定义控制台应用程序的入口点。
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
	vector<int>nums(size);
	for (int i = 0; i < size; i++) {
		input >> nums[i];
	}

	//寻找插入的位置，并将相应的元素向后移动
	for (int i = 1; i < size; i++) {
		int ind=i,num = nums[i];
		while (ind > 0 && num < nums[ind - 1]) {
			nums[ind] = nums[ind - 1];
			ind--;
		}

		nums[ind] = num;
	}

	for (int num : nums) {
		cout << num << ends;
	}
	cout << endl;
    return 0;
}

