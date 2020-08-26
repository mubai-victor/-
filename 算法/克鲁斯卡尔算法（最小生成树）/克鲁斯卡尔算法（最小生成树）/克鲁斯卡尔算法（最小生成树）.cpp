// 克鲁斯卡尔算法（最小生成树）.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "vector"
#include "fstream"
#include "queue"

using namespace std;

struct Node {
	int _src, _des;
	int _weight,_id;
	Node(int src, int des, int weight,int id) {
		_src = src, _des = des, _weight = weight,_id=id;
	}

	bool operator<(const Node&node)const {
		return _weight > node._weight;
	}
};

int find(int i,vector<int>&belong) {
	if (belong[i] == -1) {
		belong[i] = i;
	}
	else if (belong[i] != i) {
		belong[i] = find(belong[i], belong);
	}

	return belong[i];
}

bool combine(int i,int j,vector<int>&belong) {
	int fi = find(i, belong), fj = find(j, belong);
	if (fi != fj) {
		belong[fi] = belong[fj];
		return true;
	}
	else {
		return false;
	}
}

int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	int edge=0, node=0;
	input >> node >> edge;

	vector<int> belong(node, -1);
	priority_queue<Node> pQue;
	for (int i = 0; i < edge; i++) {
		int src=0, des = 0, weight = 0;
		input >> src >> des >> weight;
		pQue.push(Node(src, des, weight,i));
	}

	int cnt = 0;
	while (cnt < node - 1 && pQue.empty() != true) {
		auto &pres = pQue.top();
		int src = pres._src, des = pres._des, weight = pres._weight, id = pres._id;
		pQue.pop();

		if (combine(src,des,belong)==true) {
			cout << "src=" << src << " des=" << des << endl;
		}
	}

	getchar();
    return 0;
}

