// 基数排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
	vector<vector<int>> nums(size);
	for (int i = 0; i < size; i++) {
		int num = 0;
		
		input >> num;
		nums[i] = { num,num };
	}

	//如果所有的数字都已经变成了0，则可以结束了
	bool flag = true;
	//每一位的数字都有10中可能，因而准备10个桶
	vector<vector<vector<int>>> buckets(10);
	while (flag==true) {
		flag = false;
		for (int i = 0; i < size; i++) {
			//tail为最后一位的值
			int tail = nums[i][0] % 10;
			nums[i][0] = nums[i][0] / 10;
			//将当前的数字放到相应的桶中
			buckets[tail].push_back(nums[i]);

			//如果有一个数字当前还不是0，则可以继续运算
			if (nums[i][0] != 0) {
				flag = true;
			}
		}

		//排序完成的数字重新放回nums中
		nums.clear();
		for (int i = 0; i < 10; i++) {
			for (vector<int>&vec : buckets[i]) {
				nums.push_back(vec);
			}
			buckets[i].clear();
		}
	}

	for (vector<int>&vec : nums) {
		cout << vec[1] << ends;
	}
    return 0;
}

