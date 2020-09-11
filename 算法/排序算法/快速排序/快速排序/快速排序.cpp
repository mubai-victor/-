// ��������.cpp : �������̨Ӧ�ó������ڵ㡣
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
		//��Ϊpivot�Ѿ����м����ּ�¼�����ˣ����Կ��Խ���ߵ������ƶ����������ȱ
		while (left < right&&nums[right] >= pivot) {
			right--;
		}
		nums[left] = nums[right];

		//��Ϊ��ߵ������Ѿ��ƶ������ˣ�ԭ��λ���Ѿ����ˣ����Դ�ǰ���ƶ�һ������˲���ȱ
		while (left < right&&nums[left] <= pivot) {
			left++;
		}
		nums[right] = nums[left];
	}

	nums[left] = pivot;
	fastSort(start, left - 1, nums);
	fastSort(left + 1, end, nums);
}

