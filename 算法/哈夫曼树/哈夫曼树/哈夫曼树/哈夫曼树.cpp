// 哈夫曼树.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "queue"
#include "fstream"
#include "string"
using namespace std;

struct Node {
	int _val,_id;
	Node *_right, *_left;

	Node(int id,int val,Node*left,Node*right) {
		_id = id;
		_val = val;
		_left = left;
		_right = right;
	}

	bool operator<(const Node&node)const {
		return _val > node._val;
	}
};

struct Comp {
	bool operator ()(Node*node1,Node*node2){
		return *node1 < *node2;
	}
};
void dfs(Node*root, string&str);
int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "can not open file" << endl;
		exit(0);
	}

	int size = 0;
	input >> size;
	//读入每一个点的权值，将点按照权值的大小进行排序
	priority_queue<Node*,vector<Node*>,Comp>pQue;
	for (int i = 0; i < size; i++) {
		int weight = 0;
		input  >> weight;
		Node*node = new Node(i, weight, NULL, NULL);
		pQue.push(node);
	}

	int cnt = size;
	while (pQue.size() != 1) {
		//每次从优先队列中弹出两个权值最小的点组成一个新点放入到优先队列中，
		//新点的权值是两个点的权值之和，弹出的这两个点分别是两个点的左右子节点
		Node*node1 = pQue.top(); pQue.pop();
		Node*node2 = pQue.top(); pQue.pop();

		Node*node3 = new Node(cnt++, node1->_val + node2->_val, node1, node2);
		pQue.push(node3);
	}

	string str;
	dfs(pQue.top(), str);

    return 0;
}

//深度优先搜索，到达叶子节点的时候输出叶子节点的编码，当节点为父节点的右节点的时候
//这一位的节点的编码是1，否则是0
void dfs(Node*root,string&str) {
	if (root->_left == NULL&&root->_right == NULL) {

		cout << "id=" << root->_id << ",code=" << str << endl;
		return;
	}

	if (root->_left != NULL) {
		str.push_back('0');
		dfs(root->_left, str);
		str.pop_back();
	}
	if (root->_right != NULL) {
		str.push_back('1');
		dfs(root->_right, str);
		str.pop_back();
	}
}

