// 迪杰斯特拉.cpp : 定义控制台应用程序的入口点。
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

	//每次选择一个到目标点最近的点计入到已经访问集合中，同时用目标点到该点的距离来更新
	//未计入访问集合的点的距离，直到所有点都访问完毕。
	int last = 0,start=0;
	dist[0] = 0, vis[0] = true;
	for (int i = 1; i < node; i++) {
		int minVal = inf, minNode = 0;

		for (int j = 0; j < node; j++) {
			//已经求得最小值的点不需要再次的更新了。
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

