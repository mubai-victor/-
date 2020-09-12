#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<vector>
#include<algorithm>
#include<fstream>
#include<string>
#include<ctime>
using namespace std;

ifstream fin("/data/test_data.txt", ios::in);
ofstream fout("/projects/student/result.txt");

//有向图的邻接表结构
struct Node
{
	int v;
	Node* next;
	Node(int _v) :v(_v), next(nullptr) {};
};
struct Hnode
{
	Node *first;
};

//tarjan算法所需的数据结构
struct runTimeData
{
	bool isVisited;
	int dfn;
	int low;
	runTimeData() :isVisited(false), dfn(0), low(0) {};
};

unordered_map<int, Hnode> graph;		//如果存在边u--->v 则graph[u].contains(v)
unordered_map<int, Hnode> Sccgraph;		//强连通分量子图
unordered_map<int, Hnode> reSccgraph;	//强连通分量子图的逆边图
unordered_map<int, runTimeData> state;	//图中结点的状态
unordered_map<int, unordered_set<int>> cycleArcIn;		//记录环弧的 终点对应的起点
unordered_map<int, unordered_map<int, unordered_set<int>>> previousAns;		//逆边图遍历两层的结果
vector<int> st;							//模拟栈
vector<vector<int>> scc;				//存储强连通分量中的结点
vector<int> tmpvec;						//临时数组
unordered_set<int> mySet;
int num, curNode;

//字典序排序结果
struct vectorComp {
	bool operator() (const vector<int> &v1, const vector<int> &v2) const {
		if (v1.size() == v2.size())
			return lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
		return v1.size() < v2.size();
	}
};
set<vector<int>, vectorComp> res;

//Tarjan算法求强连通分量
void Tarjan(int x)
{
	state[x].dfn = state[x].low = ++num;
	st.push_back(x), state[x].isVisited = true;
	int node;
	for (Node *it = graph[x].first; it; it = it->next)
	{
		node = it->v;
		if (!state[node].dfn)
		{
			Tarjan(node);
			state[x].low = min(state[x].low, state[node].low);
		}
		else if (state[node].isVisited)
		{
			state[x].low = min(state[x].low, state[node].dfn);
		}
	}
	if (state[x].dfn == state[x].low)
	{
		if (st.back() == x)
		{
			st.pop_back();
			state[x].isVisited = false;
			return;
		}
		do
		{
			curNode = st.back();
			st.pop_back();
			state[curNode].isVisited = false;
			tmpvec.push_back(curNode);
			mySet.insert(curNode);
		} while (x != curNode);

		if (tmpvec.size() >= 3)
		{
			sort(tmpvec.begin(), tmpvec.end());   //从小到大排序
			for (auto it = tmpvec.begin(); it != tmpvec.end();it++)
			{
				int u = *it;
				for (Node *ptr = graph[u].first; ptr; ptr = ptr->next)
				{
					int v = ptr->v;
					if (mySet.find(v)!=mySet.end())
					{//  *iter--->(ptr->v),指针和迭代器的访问会耗时
						Node *q = new Node(v);		//保存强连通分量子图
						q->next = Sccgraph[u].first;
						Sccgraph[u].first = q;

						if (v < u)
						{//保存强连通分量中的 环弧的终点和起点,
							cycleArcIn[v].insert(u);
						}

						Node *p = new Node(u);		//保存强连通分量子图中的逆边图
						p->next = reSccgraph[v].first;
						reSccgraph[v].first = p;
					}
				}
			}
			scc.emplace_back(tmpvec);
			tmpvec.clear();
		}
		mySet.clear();
	}
}

//v:当前正在访问的结点，stNode：路径的开始结点，path:存储路径，S:标记路径上已访问的结点
void circuit(int v)
{
	st.push_back(v);
	if (st.size() >= 2 && previousAns.count(v) && previousAns[v].count(st[0]))
	{
		for (const int & e : previousAns[v][st[0]])
		{
			if (!state[e].isVisited)
			{//未访问过
				st.push_back(e);
				res.insert(st);
				st.pop_back();
			}
		}
	}
	if (st.size() == 6)
	{//6层剪枝
		st.pop_back();
		return;
	}
	state[v].isVisited = true;
	int node;
	for (Node *it = Sccgraph[v].first; it; it = it->next)
	{//在强连通分量子图中搜索
		node = it->v;
		if (node <= st[0])
			continue;
		else if (!state[node].isVisited)
			circuit(node);	//没有访问，则开始寻找回路
	}
	st.pop_back();
	state[v].isVisited = false;
}


int main()
{
	//clock_t startTime, endTime;
	//startTime = clock();
	//1.读取文件内容，构造原始图
	int u, v, w;
	char c;
	while (fin >> u >> c >> v >> c >> w)
	{//边  u---->v
		Node *q = new Node(v);		//保存原始图
		q->next = graph[u].first;
		graph[u].first = q;
	}
	//endTime = clock();
	//cout << "Read file: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;


	//2.利用tarjan算法，求强连通分量
	for (auto it = graph.begin(); it != graph.end(); it++)
	{
		if (!state[it->first].dfn)
			Tarjan(it->first);
	}

	for (auto it = cycleArcIn.begin(); it != cycleArcIn.end(); it++)
	{//从环弧的终点开始遍历
		for (const int & e : it->second)
		{//遍历环弧的起点
			for (Node *pNode = reSccgraph[e].first; pNode; pNode = pNode->next)
			{//遍历环弧起点的上一层父节点
				if (pNode->v > it->first)
					previousAns[pNode->v][it->first].insert(e);
			}
		}
	}
	//3.开始查找环
	st.clear();
	for (int i = 0; i < (int)scc.size(); i++)
	{
		for (int j = 0; j + 2< (int)scc[i].size(); j++)
		{//遍历强连通分量中属于 环弧终点的结点
			if (cycleArcIn.find(scc[i][j]) != cycleArcIn.end())
				circuit(scc[i][j]);
		}
	}

	//endTime = clock();
	//cout << "Calculation finished: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	//4.输出结果
	fout << res.size() << endl;
	for (auto & v : res)
	{
		int j = 0;
		for (; j + 1< (int)v.size(); j++)
		{
			fout << v[j] << ",";
		}
		fout << v[j] << endl;
	}
	fout.close();

	//endTime = clock();
	//cout << "Output file: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	return 0;
}





