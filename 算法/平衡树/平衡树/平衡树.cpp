// 平衡树.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "algorithm"
using namespace std;

struct Node {
	int _height;
	int _val;
	Node*_left, *_right;
	Node(int val, int height, Node*left, Node*right) {
		_val = val;
		_height = height;
		_left = left, _right = right;
	}
};

void balance(Node*&root);
bool insert(Node*&root,int val);
bool erase(Node*&root, int val);
Node* query(Node*root,int val);
Node* leftRotate(Node*root);
Node* rightRotate(Node*root);

int main()
{
	ifstream input("input.txt");
	if (!input) {
		cout << "Can not open file." << endl;
		exit(0);
	}

	Node*tree=NULL;
	while (input) {
		int val = 0;
		input >> val;
		insert(tree, val);
	}

	cout << "Enter the num you want to query." << endl;
	while (1) {
		int val = 0;
		cin >> val;
		cout << ((NULL!=query(tree, val))?"Yes":"No")<<endl;
	}

    return 0;
}

//左树高为正，右树高为负
int getBalanceFactor(Node*root) {
	return (root->_left==NULL?0:root->_left->_height) - (root->_right==NULL?0:root->_right->_height);
}

//更新树的高度
int getHeight(Node*root) {
	return max(root->_right == NULL ? 0 : root->_right->_height, root->_left == NULL ? 0 : root->_left->_height) + 1;
}

//向平衡树中插入一个新的元素
bool insert(Node*&root, int val) {
	if (root == NULL) {
		root = new Node(val, 1, NULL, NULL);
		return true;
	}
	
	bool res = true;
	//树中已经有这个值了，返回错误
	if (root->_val == val) {
		return false;
	}
	else if (root->_val < val) {
		res = insert(root->_right, val);
	}
	else {
		res = insert(root->_left, val);
	}
	root->_height = getHeight(root);
	
	balance(root);
	return res;
}

//查询树中是否含有val
Node* query(Node*root, int val) {
	if (root == NULL) {
		return NULL;
	}

	if (root->_val == val) {
		return root;
	}
	else if (root->_val < val) {
		return query(root->_right, val);
	}
	else {
		return query(root->_left, val);
	}
}

//从平衡树中删除一个元素，如果有这个元素返回true，没有返回false
bool erase(Node*&root, int val) {
	if (root == NULL) {
		return false;
	}

	if (root->_val == val) {
		//如果是一个叶子节点，不用调整高度
		if (root->_right == NULL&&root->_left == NULL) {
			root = NULL;
			return true;
		}
		//如果只有一个节点，因为是平衡树，两端高度差不大于2，所以有节点一端高度为1
		else if (root->_left != NULL || root->_left != NULL) {
			root = root->_left == NULL ? root->_right : root->_left;
			return true;
		}
		//如果有两个节点
		else {
			Node*node = root->_left;
			while (node->_right != NULL) {
				node = node->_right;
			}

			root->_val = node->_val;
			//递归删除
			erase(root->_left, root->_val);
		}
	}
	else if (root->_val < val) {
		erase(root->_right, val);
	}
	else {
		erase(root->_left, val);
	}
	root->_height = getHeight(root);
	balance(root);
	return true;
}

//左转以平衡
Node* leftRotate(Node*root){
	Node*res = root->_right;
	root->_right = res->_left;
	res->_left = root;

	root->_height = getHeight(root);
	res->_height = getHeight(res);
	return res;
}

//右转以平衡
Node* rightRotate(Node*root) {
	Node*res = root->_left;
	root->_left = res->_right;
	res->_right = root;

	root->_height = getHeight(root);
	res->_height = getHeight(res);
	return res;
}

void balance(Node*&root) {
	int factor = getBalanceFactor(root);
	if (factor == -2) {
		//RL型树
		if (getBalanceFactor(root->_right) == 1) {
			root->_right = rightRotate(root->_right);
			root = leftRotate(root);
		}
		//RR型树
		else {
			root = leftRotate(root);
		}
	}
	else if (factor == 2) {
		//LL型树
		if (getBalanceFactor(root->_left) == 1) {
			root = rightRotate(root);
		}
		//LR型树
		else {
			root->_left = leftRotate(root->_left);
			root = rightRotate(root);
		}
	}
}
