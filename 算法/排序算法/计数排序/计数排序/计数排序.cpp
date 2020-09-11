// 计数排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <assert.h>

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

	//找到数据的最大最小值，作为计数排序的上下限
	int minVal = *min_element(nums.begin(), nums.end()), maxVal = *max_element(nums.begin(), nums.end());
	int len = maxVal - minVal + 1;
	vector<int> cnt(len, 0);
	for (int i = 0; i < size; i++) {
		cnt[nums[i] - minVal]++;
	}
	
	vector<int> res;
	for (int i = 0; i < len; i++) {
		while (cnt[i]-- != 0) {
			res.push_back(i + minVal);
		}
	}

	for (int num : res) {
		cout << num << ends;
	}
    return 0;
}

