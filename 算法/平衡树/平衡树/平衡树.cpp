// ƽ����.cpp : �������̨Ӧ�ó������ڵ㡣
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

//������Ϊ����������Ϊ��
int getBalanceFactor(Node*root) {
	return (root->_left==NULL?0:root->_left->_height) - (root->_right==NULL?0:root->_right->_height);
}

//�������ĸ߶�
int getHeight(Node*root) {
	return max(root->_right == NULL ? 0 : root->_right->_height, root->_left == NULL ? 0 : root->_left->_height) + 1;
}

//��ƽ�����в���һ���µ�Ԫ��
bool insert(Node*&root, int val) {
	if (root == NULL) {
		root = new Node(val, 1, NULL, NULL);
		return true;
	}
	
	bool res = true;
	//�����Ѿ������ֵ�ˣ����ش���
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

//��ѯ�����Ƿ���val
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

//��ƽ������ɾ��һ��Ԫ�أ���������Ԫ�ط���true��û�з���false
bool erase(Node*&root, int val) {
	if (root == NULL) {
		return false;
	}

	if (root->_val == val) {
		//�����һ��Ҷ�ӽڵ㣬���õ����߶�
		if (root->_right == NULL&&root->_left == NULL) {
			root = NULL;
			return true;
		}
		//���ֻ��һ���ڵ㣬��Ϊ��ƽ���������˸߶Ȳ����2�������нڵ�һ�˸߶�Ϊ1
		else if (root->_left != NULL || root->_left != NULL) {
			root = root->_left == NULL ? root->_right : root->_left;
			return true;
		}
		//����������ڵ�
		else {
			Node*node = root->_left;
			while (node->_right != NULL) {
				node = node->_right;
			}

			root->_val = node->_val;
			//�ݹ�ɾ��
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

//��ת��ƽ��
Node* leftRotate(Node*root){
	Node*res = root->_right;
	root->_right = res->_left;
	res->_left = root;

	root->_height = getHeight(root);
	res->_height = getHeight(res);
	return res;
}

//��ת��ƽ��
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
		//RL����
		if (getBalanceFactor(root->_right) == 1) {
			root->_right = rightRotate(root->_right);
			root = leftRotate(root);
		}
		//RR����
		else {
			root = leftRotate(root);
		}
	}
	else if (factor == 2) {
		//LL����
		if (getBalanceFactor(root->_left) == 1) {
			root = rightRotate(root);
		}
		//LR����
		else {
			root->_left = leftRotate(root->_left);
			root = rightRotate(root);
		}
	}
}
