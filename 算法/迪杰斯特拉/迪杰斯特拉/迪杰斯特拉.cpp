// �Ͻ�˹����.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "fstream"
#include "vector"
#include "utility"
#include "iostream"

using namespace std;
const int inf = 0x3fffff;
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	int node = 0, edge = 0;
	input >> node >> edge;

	vector<int> dist(node, inf), vis(node, false);
	vector<vector<int>> graph(node,vector<int>(node,inf));
	int minWeight = inf, minNode = 0;
	for (int i = 0; i < edge; i++) {
		int src = 0, des = 0, weight = 0;
		input >> src >> des >> weight;
		graph[des][src] = weight;
		graph[src][des] = weight;
	}

	//ÿ��ѡ��һ����Ŀ�������ĵ���뵽�Ѿ����ʼ����У�ͬʱ��Ŀ��㵽�õ�ľ���������
	//δ������ʼ��ϵĵ�ľ��룬ֱ�����е㶼������ϡ�
	int last = 0,start=0;
	dist[0] = 0, vis[0] = true;
	for (int i = 1; i < node; i++) {
		int minVal = inf, minNode = 0;

		for (int j = 0; j < node; j++) {
			//�Ѿ������Сֵ�ĵ㲻��Ҫ�ٴεĸ����ˡ�
			if (vis[j] == true) {
				continue;
			}
			if (dist[last] + graph[last][j] < dist[j]) {
				dist[j] = dist[last] + graph[last][j];
			}
			if (dist[j] < minVal) {
				minVal=dist[j];
				minNode = j;
			}
		}

		last = minNode;
		vis[last] = true;
	}

	for (int i = 0; i < node; i++) {
		cout << "node0 to node" << i << ":" << dist[i] << endl;
	}

    return 0;
}

