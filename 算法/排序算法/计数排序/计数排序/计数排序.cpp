// ��������.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//�ҵ����ݵ������Сֵ����Ϊ���������������
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

