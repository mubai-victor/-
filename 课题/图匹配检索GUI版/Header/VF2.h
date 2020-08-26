#ifndef _VF2_H_
#define _VF2_H_

#include <vector>
#include <map>
#include <set>
#include <list>

#define OUT -2 /*表示不在搜索集合中*/
#define IN -1 /*表示在搜索集合中*/

#define SwMateSYMMETRIC 0x01
#define SwMateCAMFOLLOWER 0x02
#define SwMateGEAR 0x04
#define SwMateRACKPINION 0x08
#define SwMatePATH 0x10
#define SwMateLOCK 0x20
#define SwMateSCREM 0x40
#define SwMateWIDTH 0x80

#define CNT_RELATIONS 25

struct Entry {
	int _tag;
	int _count;

	bool operator <(const Entry& elem)const { return _count > elem._count; }
};

struct Edge
{
	int id2;
	int label;
};
//邻接链表结构，不过是用vector来实现
struct Graph
{
	std::vector<int> vID;/*相同的位置的元素表示的同一个端点的id和label*/
	std::vector<std::vector<int> > vAdjacencyEdge;
	std::vector<std::vector<int>> _adjacencyPoint;
	//外面的大vector< >，为每一个节点保存一个邻接表，一个图中有多少个节点，vAdjacencyEdge的size就是多少
	//内层的vector<Edge>存放Edge[id2,label]组元，表示每个节点对应的兄弟节点id以及这两个节点间的边的label，
	//内层的vector<Edge>大小由每个节点的兄弟数量决定（这里所谓的兄弟，就是指“邻接点”）
	//因为可行pair(m,n)就是从当前状态M(s)的邻接点中寻找的，所以该结构能够加快搜索速度
	std::vector<std::vector<std::vector<int>>> _edgeCntRelations;//统计两个点之间的边的不同装配关系的个数
	std::map<unsigned long long, int> _subGraph;/*long long类型表示子图，int类型表示这个子图在当前的图结构中出现的次数*/
	static std::map<unsigned long long, std::list<Entry>> _list;
};

//match结构，对应论文中提到的core_1 and core_2,
//whose dimensions correspond to the number of nodes in G1 and G2
struct Match
{
	//int *dbMATCHqu; //存储DB中的节点id和与之match的QU中的节点id
	//int *quMATCHdb; //存储QU中的节点id和与之match的DB中的节点id
	//使用map编程更方便，查找速度更快!
	std::vector<int> _dbMATCHqu;
	std::vector<int> _quMATCHdb;
	void clear() { _dbMATCHqu.clear(); _quMATCHdb.clear(); }
};

class GraphMatch {
private:
	Match _result;//两个图匹配的结果，也即A装配体的1号零件和B装配体的2号零件向匹配
	int _depth;//两个图匹配的深度，即两个图匹配的点的个数
	int _qu;//在这一对匹配的结果中，输入的装配体的编号
	int _db;//在这一对匹配的结果中，位于数据库中的装配体的编号
public:
	GraphMatch() { _depth = 0; }
	std::vector<Graph *> readGraph(const char *filename);
	bool feasibilityRules(Graph *quG, Graph *dbG, Match match, int quG_vID, int dbG_vID);
	void recursivelyMatchGraph(Graph *quG, Graph *dbG, int preQu, int preDb, int depth, Match &match);
	void setQuery(int qu) { _qu = qu;}
	void setDatabase(int db) { _db = db; }
	int getQuery() { return _qu; }
	int getDatabase() { return _db; }
	int getDepth() { return _depth; }
	int subGraphMatch(Graph *quG,Graph *dbG);
	Match getResult();
	void clear() { _result.clear(); }
};

struct RetrivalRes {
	int _graphNum;/*数据库中的装配体的编号*/
	int _depth;/*匹配的层数*/
	double _similarity;/*考虑匹配零件之后的相似度*/
	bool operator<(const RetrivalRes &elem)const {
		if (_depth==elem._depth)
			return _similarity < elem._similarity;
		else
			return _depth > elem._depth;
	}
};

class Retrival {
private:
	std::vector<std::vector<std::vector<int>>> _histogram;
	std::vector<std::vector<std::vector<double>>> _box;
	std::vector<RetrivalRes> _result;
public:
	void getShapeDistribution(char *fileName);
	void calculate(std::vector<GraphMatch> &maps, std::vector<Graph*>&graph);
	std::vector<RetrivalRes> getResult() {return _result;}
	void clear() { _result.clear(); };
};
int getRotation(int rotate);
int getTransform(int transform);
long getShapeDistance(std::vector<int>vect1, std::vector<int>vect2);
double getPosDistance(std::vector<double>&vect1,std::vector<double>&vect2);
double getRelationDistance(int pres, int &cnt, std::vector<std::vector<int>> &graph, std::vector<int> &quMATCHdb,
	std::vector<std::vector<std::vector<int>>> &edgeQu, std::vector<std::vector<std::vector<int>>> &edgeDb, std::vector<std::vector<bool>> &vis);
void normalization(std::vector<std::vector<double>> &vect);

#endif // _VF2_H_
