// Ͱ����.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

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
		int num = 0;
		input >> num;
		nums[i] = num;
	}

	int maxVal = *max_element(nums.begin(), nums.end()), minVal = *min_element(nums.begin(), nums.end());
	//����4��Ͱ��ÿ��Ͱ�Ĵ�СΪ(maxVal - minVal) / (cnt - 1)
	int cnt = 4, gap = (maxVal - minVal) / (cnt - 1);
	vector<vector<int>> buckets(cnt);
	//��Ԫ�طŵ���Ӧ��Ͱ��
	for (int i = 0; i < size; i++) {
		int index = (nums[i] - minVal) / gap;
		buckets[index].push_back(nums[i]);
	}
	//Ͱ�ڵ�Ԫ�ص���������
	for (int i = 0; i < cnt; i++) {
		sort(buckets[i].begin(), buckets[i].end());
	}

	//�����ս������res������
	vector<int> res;
	for (int i = 0; i < cnt; i++) {
		res.insert(res.end(), buckets[i].begin(), buckets[i].end());
	}
	for (int num : res) {
		cout << num << ends;
	}
    return 0;
}

