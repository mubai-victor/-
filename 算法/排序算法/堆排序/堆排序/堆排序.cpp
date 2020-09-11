// ������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;

void build(int index, int len, vector<int>&nums);
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	int size = 0;
	input >> size;
	vector<int> nums(size, 0);
	for (int i = 0; i < size; i++) {
		input >> nums[i];
	}

	//��ʼ����һ���󶥶�
	for (int i = size / 2; i >= 0; i--) {
		build(i, size, nums);
	}
	//ÿ�ΰѴ󶥶�������Ԫ�طŵ�����һ��Ԫ���У���Ϊ�ź����Ԫ��
	for (int i = size - 1; i > 0; i--) {
		swap(nums[i], nums[0]);
		build(0, i, nums);
	}

	for (int num : nums) {
		cout << num << ends;
	}
	cout << endl;
    return 0;
}

void build(int parent,int len,vector<int>&nums) {
	int maxInd = parent;
	int left = 2 * parent + 1, right = 2 * parent + 2;
	//Ѱ�����������ӽڵ��бȽϴ����һ���͸��ڵ���бȽϣ����ߺ͸��ڵ���бȽϣ�������
	if (left < len) {
		maxInd = nums[maxInd] > nums[left] ? maxInd : left;
	}
	if (right < len) {
		maxInd = nums[maxInd] > nums[right] ? maxInd : right;
	}
	

	if (maxInd != parent) {
		swap(nums[parent], nums[maxInd]);
		build(maxInd, len, nums);
	}
}

