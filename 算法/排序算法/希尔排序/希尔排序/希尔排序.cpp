// ϣ������.cpp : �������̨Ӧ�ó������ڵ㡣
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
	vector<int> nums(size);
	for (int i = 0; i < size; i++) {
		input >> nums[i];
	}

	//ÿ��������interval��Ԫ�ص�Ԫ��
	for (int interval = size/2; interval > 0; interval /= 2) {
		//��start��ʼ��ÿ��interval��Ԫ�ؽ������򣬲��õ�ѡ������
		for (int start = interval; start < size; start++) {
			int index=start,val = nums[start];
			//Ѱ�Ҳ���λ�ã�ͬʱ��Ԫ�غ���
			while (index - interval >= 0 && val > nums[index - interval]) {
				nums[index] = nums[index - interval];
				index -= interval;
			}
			nums[index] = val;
		}
	}

	for (int num : nums) {
		cout << num << ends;
	}
	cout << endl;
    return 0;
}

