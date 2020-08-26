// ����ķ�㷨����С��������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"
using namespace std;
//prim�㷨�����ΪU(�������Ľڵ�),��V(δ�������Ľڵ�)��ÿ�δӱ߼���ѡ��e(u,v)����u��U�У�v��V��
//��Ȩֵ��С�ı߼��뵽U�У�ֱ��VΪ��
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	//lowCost[i]��ʾ�ڵ�i���������е�ı���Ȩ����С�ıߣ����i�Ѿ����뵽�����ˣ�
	//lowCost[i]=0��
	int node = 0, edge = 0;
	input >> node >> edge;
	vector<int> lowCost(node, INT_MAX);
	vector<vector<int>> graph(node,vector<int>(node,INT_MAX));

	for (int i = 0; i < edge; i++) {
		int start = 0, end = 0, weight = 0;
		input >> start >> end >> weight;
		graph[start][end] = graph[end][start] = weight;
	}

	//��ʼ�������еĵ���0����lowCost[0]=0,��ʾ�Ѿ���������
	int last = 0;
	lowCost[0] = 0;
	
	for (int i = 1; i < node; i++) {
		int minWeight = INT_MAX, minNode = 0;
		//���½ڵ���뵽���п��ܻ�ʹÿ���㵽���е�ıߵ���СȨ�����仯�����Ը���lowCost��ͬʱ
		//ѡ��lowCost��Ȩ����С�ı�
		for (int j = 0; j < node; j++) {
			if (lowCost[j] == 0) {
				continue;
			}

			//����lowCost
			if (lowCost[j] > graph[j][last]) {
				lowCost[j] = graph[j][last];
			}

			//���±�Ȩ��Сֵ
			if (lowCost[j] < minWeight) {
				minWeight = lowCost[j];
				minNode = j;
			}
		}

		cout << "node:" << minNode << endl;
		//minNode���뵽����
		lowCost[minNode] = 0;
		last = minNode;
	}

    return 0;
}

