// ��������.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//������е����ֶ��Ѿ������0������Խ�����
	bool flag = true;
	//ÿһλ�����ֶ���10�п��ܣ����׼��10��Ͱ
	vector<vector<vector<int>>> buckets(10);
	while (flag==true) {
		flag = false;
		for (int i = 0; i < size; i++) {
			//tailΪ���һλ��ֵ
			int tail = nums[i][0] % 10;
			nums[i][0] = nums[i][0] / 10;
			//����ǰ�����ַŵ���Ӧ��Ͱ��
			buckets[tail].push_back(nums[i]);

			//�����һ�����ֵ�ǰ������0������Լ�������
			if (nums[i][0] != 0) {
				flag = true;
			}
		}

		//������ɵ��������·Ż�nums��
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

