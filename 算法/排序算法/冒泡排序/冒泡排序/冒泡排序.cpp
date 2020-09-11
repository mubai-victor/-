// 冒泡排序.cpp : 定义控制台应用程序的入口点。
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

	int cnt = 0;
	input >> cnt;
	vector<int> nums(cnt);
	for (int i = 0; i < cnt; i++) {
		input >> nums[i];
	}

	bool flag = true;
	for (int i = 0; i < cnt&&flag == true; i++) {
		flag = false;
		for (int j = 1; j < cnt; j++) {
			if (nums[j - 1] > nums[j]) {
				swap(nums[j - 1], nums[j]);
				flag = true;
			}
		}
	}

	for (int num : nums) {
		cout << num << ends;
	}
	return 0;
}

