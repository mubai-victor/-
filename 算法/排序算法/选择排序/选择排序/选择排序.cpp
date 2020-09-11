// 选择排序.cpp : 定义控制台应用程序的入口点。
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
	}

	int cnt = 0;
	input >> cnt;
	vector<int> nums(cnt);
	for (int i = 0; i < cnt; i++) {
		input >> nums[i];
	}

	for (int i = 0; i < cnt; i++) {
		int minInd = i;
		for (int j = i + 1; j < cnt; j++) {
			if (nums[j] < nums[minInd]) {
				swap(nums[j], nums[minInd]);
			}
		}
	}

	for (int num : nums) {
		cout << num << ends;
	}
    return 0;
}

