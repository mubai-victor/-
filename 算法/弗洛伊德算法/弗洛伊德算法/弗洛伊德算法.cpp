// 弗洛伊德算法.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "vector"
#include "fstream"

using namespace std;
const int inf = 0x3fffff;
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
	}

	int node = 0, edge = 0;
	input >> node >> edge;
	vector<vector<int>> graph(node, vector<int>(node, inf));
	for (int i = 0; i < edge; i++) {
		int src = 0, des = 0, weight = 0;
		input >> src >> des >> weight;
		graph[src][des] = weight;
		graph[des][src] = weight;
	}

	for (int i = 0; i < node; i++) {
		for (int j = 0; j < node; j++) {
			if (i == j) {
				graph[i][j] = 0;
				continue;
			}
			for (int k = 0; k < node; k++) {
				if (graph[i][j] > graph[i][k] + graph[k][j]) {
					graph[i][j] = graph[j][i] = graph[i][k] + graph[k][j];
				}
			}
		}
	}

	for (int i = 0; i < node; i++) {
		cout << "node0 to node" << i << ":" << graph[0][i] << endl;
	}
    return 0;
}

