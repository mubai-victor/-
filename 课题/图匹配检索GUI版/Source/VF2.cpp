//#include "stdafx.h"
#include "VF2.h"
#include "fstream"
#include "iostream"
#include "time.h"
#include "stdlib.h"
#include "algorithm"
#include "queue"
#include "numeric"
#include "string"

#define SIZE 400
int Count = 0;
long start, end;

std::map<unsigned long long, std::list<Entry>> Graph::_list;

//��ȡ�ļ���ͼ����Ϣ�����ļ���ͼ�Ĵ洢��ʽΪ��ͼ�ж���ĸ���\n��head,tail,nTnRSWK(��ѡ)��\n��head,tail,nTnRSWK(��ѡ)��......
//label��9-12λ��¼ת�����ɶȵĸ�����13-16��¼ƽ�����ɶȸ�����1-8��¼������Լ��
std::vector<Graph *> GraphMatch::readGraph(const char *filename)
{
	std::ifstream file;
	std::vector<Edge> v;
	std::vector<Graph *> result;
	Edge edge;
	Entry entry;
	Graph * graph = NULL;
	std::string work;
	std::string assist;
	std::string val;
	int id, label, id2, count;
	unsigned long long value;
	char buff[SIZE];

	file.open(filename);
	if (!file) {
		std::cout << "Can not open the file." << std::endl;
		exit(0);
	}

	file.getline(buff, SIZE);
	work.assign(buff);
	while (file)
	{
		if (work.size() < 4)/*�����ǰ�ַ����ĳ���С��4��˵����ǰ����һ���µ�װ����Ŀ�ʼ�������ֱ�־װ����������ĸ���*/
		{
			int count = atoi(work.c_str());/*���㵱ǰװ����������ĸ���*/

			if (graph != NULL) {/*���graphָ�벻Ϊ�գ��������result��*/
				result.push_back(graph);
			}

			graph = new Graph;
			graph->vAdjacencyEdge.resize(count);/*���ó�Ա��������Ϊ����ĸ�����ÿһ����Ա�洢�������װ���ϵ�������*/
			graph->_edgeCntRelations.resize(count);
			for (int i = 0; i < count; i++) {
				graph->vAdjacencyEdge[i].resize(count);
				graph->_edgeCntRelations[i] = std::vector<std::vector<int>>(count, std::vector<int>(CNT_RELATIONS, 0));
				for (int j = 0; j < count; j++) {
					graph->vAdjacencyEdge[i][j] = -1;
				}
			}
			graph->_adjacencyPoint.resize(count);

			file.getline(buff, SIZE);
			work.assign(buff);
			while (work.empty() != true) {
				assist = work.substr(0, work.find(' '));
				val = assist.substr(assist.find('(') + 1, assist.find(')')-assist.find('(') - 1);
				count = atoi(val.c_str());
				val = assist.substr(0, assist.find('('));
				value = atoll(val.substr(0, 8).c_str());
				value *= 10E9;
				val = val.substr(9);
				value += atoll(val.c_str());

				assist = work.substr(work.find(' ') + 1);
				work = assist;
				
				entry._count = count;
				graph->_subGraph[value] = count;
				entry._tag = result.size();
				Graph::_list[value].push_back(entry);
			}
		}
		else {
			work = work.substr(1, work.size() - 2);/*ȥ����������*/
			assist = work.substr(0, work.find(','));/*��ȡ��һ��������֮ǰ�����ݣ�Ϊ�ߵĵ�һ���˵�*/
			id = atoi(assist.c_str());/*ת��Ϊ����*/
			work = work.substr(work.find(',') + 1);/*��ȡ��һ��������֮����Ӵ�*/
			assist = work.substr(0, work.find(','));/*��ȡ�ڶ���������֮ǰ���Ӵ���Ϊ�ߵĵڶ����˵�*/
			id2 = atoi(assist.c_str());/*ת��Ϊ����*/
			work = work.substr(work.find(',') + 1);/*��ȡ�ڶ���������֮����Ӵ�����¼�ߵ����ʵ��Ӵ�*/
			assist = work.substr(0,work.find(','));
			work=work.substr(work.find(',') + 1);//ʣ����Ӵ�����������װ���ϵ���������Ӵ�

			label = 0;
			label |= getTransform(assist[0] - '0');/*ƽ�����ɶȵĸ���*/
			label |= getRotation(assist[2] - '0');/*ת�����ɶȵĸ���*/
			assist = assist.substr(4);/*���������˶���������֣������ȵ��Ӵ�*/

			if (assist.find('S') != -1) {/*����жԳ�Լ��*/
				label |= SwMateSYMMETRIC;
			}
			if (assist.find('C') != -1) {/*�����͹��Լ��*/
				label |= SwMateCAMFOLLOWER;
			}
			if (assist.find('G') != -1) {/*����г���Լ��*/
				label |= SwMateGEAR;
			}
			if (assist.find('R') != -1) {/*����г��ֳ���Լ��*/
				label |= SwMateRACKPINION;
			}
			if (assist.find('P') != -1) {/*�����·��Լ��*/
				label |= SwMatePATH;
			}
			if (assist.find('L') != -1) {/*���������Լ��*/
				label |= SwMateLOCK;
			}
			if (assist.find('W') != -1) {/*���������Լ��*/
				label |= SwMateSCREM;
			}
			if (assist.find('K') != -1) {/*����п��Լ��*/
				label |= SwMateWIDTH;
			}

			edge.id2 = id2;/*���¼����ڽӱߵ�vector�д洢��Ӧ�ıߣ���Ϊ������ͼ������Ҫ���������*/
			graph->_adjacencyPoint[id].push_back(id2);
			graph->vAdjacencyEdge[id][id2] = label;

			graph->_adjacencyPoint[id2].push_back(id);
			graph->vAdjacencyEdge[id2][id] = label;

			//����ʣ����Ӵ�����ȡÿһ��ת���ϵ�ڵ�ǰ�ı��е�����
			for (int i = 0; i < CNT_RELATIONS-1; i++) {
				assist = work.substr(0, work.find(','));
				work = work.substr(work.find(',') + 1);

				graph->_edgeCntRelations[id][id2][i] = stoi(assist);
				graph->_edgeCntRelations[id2][id][i] = stoi(assist);
			}
			//���һ����Ҫ�����Ĵ���
			graph->_edgeCntRelations[id][id2][CNT_RELATIONS - 1] = stoi(work);
			graph->_edgeCntRelations[id2][id][CNT_RELATIONS - 1] = stoi(work);
		}
		file.getline(buff, SIZE);/*��ȡ��һ����*/
		work.assign(buff);
	}

	if (graph != NULL) {/*���graphָ�벻Ϊ�գ��������result��*/
		result.push_back(graph);
	}
	file.close();

	return result;
}

//��ʵ pair(quG->vID[i], dbG->vID[j])����һ����ѡpair candidate
//�жϸú�ѡpair�Ƿ�����feasibility rules
bool GraphMatch::feasibilityRules(Graph *quG, Graph *dbG, Match match, int quG_vID, int dbG_vID)
{
	int quVid, dbVid, quGadjacencyEdgeSize, dbGadjacencyEdgeSize, i, j;
	bool flag = false;

	for (int point : quG->_adjacencyPoint[quG_vID]) {
		if (match._quMATCHdb[point] >= 0) {
			if (quG->vAdjacencyEdge[point][quG_vID] == dbG->vAdjacencyEdge[dbG_vID][match._quMATCHdb[point]]) {
				flag = true;
			}
			else {
				return false;
			}
		}
	}

	return flag;
}

void  GraphMatch::recursivelyMatchGraph(Graph *quG, Graph *dbG, int preQu, int preDb, int depth, Match &match)
{
	int i, iS = quG->vAdjacencyEdge.size();
	int j, jS = dbG->vAdjacencyEdge.size();
	Count++;

	if (depth == 0) {
		for (i = 0; i < iS; i++) {
			for (j = 0; j < jS; j++) {
				match._quMATCHdb[i] = j;
				match._dbMATCHqu[j] = i;
				recursivelyMatchGraph(quG, dbG, i, j, depth + 1,match);
				if (_depth == iS || _depth == jS) {
					return;
				}
				match._quMATCHdb[i] = OUT;
				match._dbMATCHqu[j] = OUT;
			}
		}
	}
	else {
		int originI, originJ;
		std::vector<int> quOrigin = match._quMATCHdb;
		std::vector<int> dbOrigin = match._dbMATCHqu;

		for (int vertex : quG->_adjacencyPoint[preQu]) {
			if (match._quMATCHdb[vertex] == OUT) {
				match._quMATCHdb[vertex] = IN;
			}
		}
		for (int vertex : dbG->_adjacencyPoint[preDb]) {
			if (match._dbMATCHqu[vertex] == OUT) {
				match._dbMATCHqu[vertex] = IN;
			}
		}

		if (_depth < depth) {
			_result = match;
			_depth = depth;
		}
		if (_depth == iS || _depth == jS) {
			return;
		}

		for (i = 0; i < iS; ++i) { //ÿһ����û��match�ϵ�quG�Ľڵ�	
			if (match._quMATCHdb[i] == IN) {
				for (j = 0; j < jS; ++j) {
					if (match._dbMATCHqu[j] == IN) {
						if (feasibilityRules(quG, dbG, match, i, j) == true) {
							match._quMATCHdb[i] = j;
							match._dbMATCHqu[j] = i;

							recursivelyMatchGraph(quG, dbG, i, j, depth + 1, match);
							if (_depth == iS || _depth == jS) {
								return;
							}
							match._quMATCHdb[i] = -1;
							match._dbMATCHqu[j] = -1;
						}
					}
				}
			}
		}

		match._quMATCHdb = quOrigin;
		match._dbMATCHqu = dbOrigin;
	}
}

int GraphMatch::subGraphMatch(Graph * quG, Graph * dbG)
{
	int i, iS = dbG->vAdjacencyEdge.size();
	int j, jS = quG->vAdjacencyEdge.size();
	Match match;

	match._dbMATCHqu.resize(iS);
	for (i = 0; i < iS; i++) {
		match._dbMATCHqu[i] = OUT;
	}

	match._quMATCHdb.resize(jS);
	for (j = 0; j < jS; j++) {
		match._quMATCHdb[j] = OUT;
	}

	int depth = 0;

	Count = 0;
	_depth = 0;
	_result._dbMATCHqu.clear();
	_result._dbMATCHqu.clear();
	recursivelyMatchGraph(quG, dbG, 0, 0, 0, match);

	std::cout << "Count:" << Count << "                  ";

	return _depth;
}

Match GraphMatch::getResult()
{
	return _result;
}

inline int getRotation(int rotate)
{
	return rotate << 8;
}

inline int getTransform(int transform)
{
	return transform << 12;
}

void normalization(std::vector<std::vector<double>> &vect)
{
	std::vector<double> assemBox(6);

	//��ȡ����װ����İ�Χ�еĳߴ磬box[0],box[1],box[2]�����½ǵ�����꣬Ҳ������С�����꣬box[3],box[4],box[5]�����Ͻǵ������
	assemBox[0] = assemBox[1] = assemBox[2] = INT_MAX;
	assemBox[3] = assemBox[4] = assemBox[5] = INT_MIN;
	for (auto box : vect) {
		assemBox[0] = assemBox[0] < box[0] ? assemBox[0] : box[0];
		assemBox[1] = assemBox[1] < box[1] ? assemBox[1] : box[1];
		assemBox[2] = assemBox[2] < box[2] ? assemBox[2] : box[2];

		assemBox[3] = assemBox[3] > box[3] ? assemBox[3] : box[3];
		assemBox[4] = assemBox[4] > box[4] ? assemBox[4] : box[4];
		assemBox[5] = assemBox[5] > box[5] ? assemBox[5] : box[5];
	}

	//��ȡ����װ�����Χ�еĳߴ�
	double assemLen = assemBox[3] - assemBox[0], assemWid = assemBox[4] - assemBox[1], assemHi = assemBox[5] - assemBox[2];
	for (std::vector<double>&vec : vect) {

		//��ȡװ������ÿ�������������װ�����Χ���е����λ��
		vec[0] = ((vec[0] + vec[3]) / 2 - assemBox[0]) / assemLen;
		vec[1] = ((vec[1] + vec[4]) / 2 - assemBox[1]) / assemWid;
		vec[2] = ((vec[2] + vec[5]) / 2 - assemBox[2]) / assemHi;
		
		vec.resize(3);
		double total = 0;
		for (double num : vec) {
			total += num;
		}
		for (double &num : vec) {
			num = 10 * num / total;
		}		
		sort(vec.rbegin(), vec.rend());
	}

}

double getPosDistance(std::vector<double>&vect1, std::vector<double>&vect2)
{
	return sqrtf((vect1[0]-vect2[0])*(vect1[0] - vect2[0]))+sqrtf((vect1[1] - vect2[1])*(vect1[1] - vect2[1]))
			+sqrtf((vect1[2] - vect2[2])*(vect1[2] - vect2[2]));
}

double getRelationDistance(int pres, int &cnt, std::vector<std::vector<int>> &graph, std::vector<int> &quMATCHdb,
   std::vector<std::vector<std::vector<int>>> &edgeQu, std::vector<std::vector<std::vector<int>>> &edgeDb,std::vector<std::vector<bool>>&vis) {
	double val = 0.0;

	//������ǰ����ͼ�ṹ��ÿһ���ڽӵ�
	for (int next : graph[pres]) {
		//����ڽӵ�û�з��ʹ���ͬʱ��ǰ������һ��ͼ�ṹ���ж�Ӧ�ĵ㣨quMATCHdb[next]Ϊ�Ǹ�ʱ�ж�Ӧ�ĵ㣩
		if (vis[pres][next] == false&&quMATCHdb[next]>=0) {
			vis[pres][next] = vis[next][pres] = true;
			cnt++;
			//װ���ϵ�����ƶȶ���Ϊ��similarity=������������ͬ��װ���ϵ����/�������������е�װ���ϵ�ĸ���-����������ͬ��װ���ϵ������
			//��ͬ��װ���ϵ�У��ϴ�ֵ֮�ͼ�Ϊ��ĸ����Сֵ��Ϊ����
			int lo = 0, hi = 0,presDb=quMATCHdb[pres],nextDb=quMATCHdb[next];
			for (int i = 0; i < CNT_RELATIONS; i++) {
				lo = lo + (edgeQu[pres][next][i] > edgeDb[presDb][nextDb][i] ? edgeQu[pres][next][i] : edgeDb[presDb][nextDb][i]);
				hi = hi + (edgeQu[pres][next][i] < edgeDb[presDb][nextDb][i] ? edgeQu[pres][next][i] : edgeDb[presDb][nextDb][i]);
			}
			//����װ���ϵ�����ƶ�
			val = val + 1.0*hi / lo;
			//�ݹ����װ���ϵ���ƶ�
			val = val + getRelationDistance(next, cnt, graph, quMATCHdb, edgeQu, edgeDb, vis);
		}
	}

	return val;
}

long getShapeDistance(std::vector<int> vect1, std::vector<int> vect2)
{
	long result = 0;
	for (int i = 0; i < 512; i++) {
		result += abs(vect1[i] - vect2[i]);/*resultΪ���е����Ӹ߶Ȳ�ֵ�ľ���ֵ���ۼ�ֵ*/
	}

	return result;
}

void Retrival::getShapeDistribution(char * fileName)
{
	char buff[SIZE];
	int curs = 0, partNum = 0, pos = 0, partCount = 0;
	std::fstream shapeInf;
	std::string work, data;
	std::vector<std::vector<int>> shapeDis;
	std::vector<std::vector<double>> box;

	shapeInf.open(fileName);/*�򿪴��װ���������״�ֲ����ļ�*/
	if (!shapeInf) {
		std::cout << "Can not open file " << fileName << std::endl;/*�޷��򿪣���ʾ�����˳�*/
		exit(0);
	}

	shapeInf.getline(buff, SIZE);/*��ȡ�ļ���һ�У����а���װ����������ĸ���*/
	work.assign(buff);
	while (shapeInf) {/*����ļ���δ�����β*/	
		//��ʼ��shapeDis��box�Ĵ�СΪ����ĸ�����ÿһά�Ĵ�С�ֱ�Ϊ512����״�ֲ������Ӹ�������6�������Χ�������ǵ����ά���꣩
		partCount = atoi(work.substr(1).c_str());
		shapeDis.resize(partCount);
		box.resize(partCount);
		for (int i = 0; i < partCount;i++) {
			shapeDis[i].resize(512);
			box[i].resize(6);
		}

		for (int i = 0; i < partCount; i++) {
			shapeInf.getline(buff, SIZE);/*��ȡ�ļ���һ�У������������װ�����е����*/
			work.assign(buff);
			partNum = atoi(work.c_str());

			curs = 0;
			for (int j = 0; j < 16; j++) {/*ÿһ���������״�ֲ���������16��*/
				shapeInf.getline(buff, SIZE);
				work.assign(buff);

				/*ÿһ�е�������32���������ȡ31�������һ�����ݵ�������*/
				for (int k = 0; k < 31; k++) {
					pos = work.find(' ');
					data = work.substr(0, pos);
					work = work.substr(pos + 1);

					shapeDis[partNum][curs++] = atoi(data.c_str());
				}
				shapeDis[partNum][curs++]= atoi(work.c_str());
			}

			curs = 0;
			shapeInf.getline(buff, SIZE);
			//��Χ����������"box:"��ͷ����Ҫ�����ĸ��ַ�
			work.assign(buff+4);
			for (int k = 0; k < 5; k++) {
				pos = work.find(',');
				data = work.substr(0, pos);
				work = work.substr(pos + 1);
				box[partNum][curs++] = atof(data.c_str());
			}
			//���һ�������Ҳ���','�ַ��ˣ���Ҫ��������
			box[partNum][curs] = atof(work.c_str());
		}

		//����ǰ����״ͼ�ֲ��Ͱ�Χ������洢��������ʱ�Ĵ������shapeDis��box���
		_histogram.push_back(shapeDis);
		shapeDis.clear();
		normalization(box);
		_box.push_back(box);
		box.clear();

		shapeInf.getline(buff, SIZE);/*��ȡ�ļ���һ�У����а���װ����������ĸ���*/
		work.assign(buff);
	}
}

void Retrival::calculate(std::vector<GraphMatch>& maps, std::vector<Graph*>&graph)
{
	int curs = 0, depth = 0;
	RetrivalRes result;
	long accumShape = 0;
	double simPos = 0.0;
	Match match;

	for (GraphMatch graphMatch : maps) {
		result._graphNum = graphMatch.getDatabase();
		result._depth = graphMatch.getDepth();
		match = graphMatch.getResult();
		depth = graphMatch.getDepth();

		int qu = graphMatch.getQuery(), db = result._graphNum;
		for (int i = 0, j = 0; i < depth; j++) {
		//���ƥ��Ľ��Ϊ�Ǹ��������������к�����ƥ���������������������֮�����״���ƶȣ��ռ�λ�����ƶ�
			if (match._quMATCHdb[j] >= 0) {				
				accumShape += getShapeDistance(_histogram[qu][j], _histogram[db][match._quMATCHdb[j]]);
				simPos += getPosDistance(_box[qu][j],_box[db][match._quMATCHdb[j]]);
				i++;
			}
		}
		
		//����װ���ϵ�����ƶȣ���ʽ��similarity=������������ͬ��װ���ϵ����/�������������е�װ���ϵ�ĸ���-����������ͬ��װ���ϵ������
		double simRelation = 0.0;		//result._similarity = 0.5*simPos + 0.5*simShape + 0.5*simRelation; ;
		//cnt�Ƕ�ƥ��ıߵļ��������Ҫ��ƽ���ıߵ�װ���ϵ���ƶȣ����ܵ�װ���ϵ���ƶ�
		int cnt = 0, size = graph[qu]->vAdjacencyEdge.size();
		std::vector<std::vector<bool>> vis(size, std::vector<bool>(size,false));
		//����quͼ�ṹ��ÿһ���㣬���û�б����ʹ���������dbͼ����ƥ��ĵ㣨quMATCHdb[i] >= 0���������װ���ϵ�����ƶȵļ���
		for (int i = 0; i < size; i++) {
			if (match._quMATCHdb[i] >= 0) {
				simRelation = simRelation + getRelationDistance(i, cnt, graph[qu]->_adjacencyPoint, match._quMATCHdb,
					graph[qu]->_edgeCntRelations, graph[db]->_edgeCntRelations, vis);
			}
		}
		simRelation = 5.0*cnt / simRelation;


		double simShape = 1.0*accumShape / 1048576;

		result._similarity = 0.5*simPos + 0.25*simShape + 0.25*simRelation;
		_result.push_back(result);
	}

	sort(_result.begin(), _result.end());
}
