#ifndef _VF2_H_
#define _VF2_H_

#include <vector>
#include <map>
#include <set>
#include <list>

#define OUT -2 /*��ʾ��������������*/
#define IN -1 /*��ʾ������������*/

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
//�ڽ�����ṹ����������vector��ʵ��
struct Graph
{
	std::vector<int> vID;/*��ͬ��λ�õ�Ԫ�ر�ʾ��ͬһ���˵��id��label*/
	std::vector<std::vector<int> > vAdjacencyEdge;
	std::vector<std::vector<int>> _adjacencyPoint;
	//����Ĵ�vector< >��Ϊÿһ���ڵ㱣��һ���ڽӱ�һ��ͼ���ж��ٸ��ڵ㣬vAdjacencyEdge��size���Ƕ���
	//�ڲ��vector<Edge>���Edge[id2,label]��Ԫ����ʾÿ���ڵ��Ӧ���ֵܽڵ�id�Լ��������ڵ��ıߵ�label��
	//�ڲ��vector<Edge>��С��ÿ���ڵ���ֵ�����������������ν���ֵܣ�����ָ���ڽӵ㡱��
	//��Ϊ����pair(m,n)���Ǵӵ�ǰ״̬M(s)���ڽӵ���Ѱ�ҵģ����Ըýṹ�ܹ��ӿ������ٶ�
	std::vector<std::vector<std::vector<int>>> _edgeCntRelations;//ͳ��������֮��ıߵĲ�ͬװ���ϵ�ĸ���
	std::map<unsigned long long, int> _subGraph;/*long long���ͱ�ʾ��ͼ��int���ͱ�ʾ�����ͼ�ڵ�ǰ��ͼ�ṹ�г��ֵĴ���*/
	static std::map<unsigned long long, std::list<Entry>> _list;
};

//match�ṹ����Ӧ�������ᵽ��core_1 and core_2,
//whose dimensions correspond to the number of nodes in G1 and G2
struct Match
{
	//int *dbMATCHqu; //�洢DB�еĽڵ�id����֮match��QU�еĽڵ�id
	//int *quMATCHdb; //�洢QU�еĽڵ�id����֮match��DB�еĽڵ�id
	//ʹ��map��̸����㣬�����ٶȸ���!
	std::vector<int> _dbMATCHqu;
	std::vector<int> _quMATCHdb;
	void clear() { _dbMATCHqu.clear(); _quMATCHdb.clear(); }
};

class GraphMatch {
private:
	Match _result;//����ͼƥ��Ľ����Ҳ��Aװ�����1�������Bװ�����2�������ƥ��
	int _depth;//����ͼƥ�����ȣ�������ͼƥ��ĵ�ĸ���
	int _qu;//����һ��ƥ��Ľ���У������װ����ı��
	int _db;//����һ��ƥ��Ľ���У�λ�����ݿ��е�װ����ı��
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
	int _graphNum;/*���ݿ��е�װ����ı��*/
	int _depth;/*ƥ��Ĳ���*/
	double _similarity;/*����ƥ�����֮������ƶ�*/
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
