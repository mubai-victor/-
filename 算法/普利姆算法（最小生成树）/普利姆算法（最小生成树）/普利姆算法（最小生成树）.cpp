// 普利姆算法（最小生成树）.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "vector"
using namespace std;
//prim算法将点分为U(加入树的节点),和V(未加入树的节点)，每次从边集中选择e(u,v)满足u在U中，v在V中
//中权值最小的边加入到U中，直到V为空
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	//lowCost[i]表示节点i到树中所有点的边中权重最小的边，如果i已经加入到树中了，
	//lowCost[i]=0；
	int node = 0, edge = 0;
	input >> node >> edge;
	vector<int> lowCost(node, INT_MAX);
	vector<vector<int>> graph(node,vector<int>(node,INT_MAX));

	for (int i = 0; i < edge; i++) {
		int start = 0, end = 0, weight = 0;
		input >> start >> end >> weight;
		graph[start][end] = graph[end][start] = weight;
	}

	//起始加入树中的点是0，置lowCost[0]=0,表示已经加入树中
	int last = 0;
	lowCost[0] = 0;
	
	for (int i = 1; i < node; i++) {
		int minWeight = INT_MAX, minNode = 0;
		//有新节点加入到树中可能会使每个点到树中点的边的最小权发生变化，所以更新lowCost，同时
		//选择lowCost中权重最小的边
		for (int j = 0; j < node; j++) {
			if (lowCost[j] == 0) {
				continue;
			}

			//更新lowCost
			if (lowCost[j] > graph[j][last]) {
				lowCost[j] = graph[j][last];
			}

			//更新边权最小值
			if (lowCost[j] < minWeight) {
				minWeight = lowCost[j];
				minNode = j;
			}
		}

		cout << "node:" << minNode << endl;
		//minNode加入到树中
		lowCost[minNode] = 0;
		last = minNode;
	}

    return 0;
}

