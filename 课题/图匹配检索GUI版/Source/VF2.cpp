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

//读取文件中图的信息，在文件中图的存储方式为：图中顶点的个数\n（head,tail,nTnRSWK(可选)）\n（head,tail,nTnRSWK(可选)）......
//label的9-12位记录转动自由度的个数。13-16记录平移自由度个数，1-8记录其他的约束
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
		if (work.size() < 4)/*如果当前字符串的长度小于4，说明当前行是一个新的装配体的开始，用数字标志装配体中零件的个数*/
		{
			int count = atoi(work.c_str());/*计算当前装配体中零件的个数*/

			if (graph != NULL) {/*如果graph指针不为空，则将其放入result中*/
				result.push_back(graph);
			}

			graph = new Graph;
			graph->vAdjacencyEdge.resize(count);/*将该成员变量设置为零件的个数，每一个成员存储与零件有装配关系的零件。*/
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
			work = work.substr(1, work.size() - 2);/*去除外层的括号*/
			assist = work.substr(0, work.find(','));/*提取第一个‘，’之前的内容，为边的第一个端点*/
			id = atoi(assist.c_str());/*转化为数字*/
			work = work.substr(work.find(',') + 1);/*提取第一个‘，’之后的子串*/
			assist = work.substr(0, work.find(','));/*提取第二个‘，’之前的子串，为边的第二个端点*/
			id2 = atoi(assist.c_str());/*转化为数字*/
			work = work.substr(work.find(',') + 1);/*提取第二个‘，’之后的子串，记录边的性质的子串*/
			assist = work.substr(0,work.find(','));
			work=work.substr(work.find(',') + 1);//剩余的子串，包含各种装配关系的数量的子串

			label = 0;
			label |= getTransform(assist[0] - '0');/*平移自由度的个数*/
			label |= getRotation(assist[2] - '0');/*转动自由度的个数*/
			assist = assist.substr(4);/*包含其他运动副，如齿轮，螺旋等的子串*/

			if (assist.find('S') != -1) {/*如果有对称约束*/
				label |= SwMateSYMMETRIC;
			}
			if (assist.find('C') != -1) {/*如果有凸轮约束*/
				label |= SwMateCAMFOLLOWER;
			}
			if (assist.find('G') != -1) {/*如果有齿轮约束*/
				label |= SwMateGEAR;
			}
			if (assist.find('R') != -1) {/*如果有齿轮齿条约束*/
				label |= SwMateRACKPINION;
			}
			if (assist.find('P') != -1) {/*如果有路径约束*/
				label |= SwMatePATH;
			}
			if (assist.find('L') != -1) {/*如果有锁定约束*/
				label |= SwMateLOCK;
			}
			if (assist.find('W') != -1) {/*如果有螺纹约束*/
				label |= SwMateSCREM;
			}
			if (assist.find('K') != -1) {/*如果有宽度约束*/
				label |= SwMateWIDTH;
			}

			edge.id2 = id2;/*向记录点的邻接边的vector中存储相应的边，因为是无向图，所以要添加两条边*/
			graph->_adjacencyPoint[id].push_back(id2);
			graph->vAdjacencyEdge[id][id2] = label;

			graph->_adjacencyPoint[id2].push_back(id);
			graph->vAdjacencyEdge[id2][id] = label;

			//处理剩余的子串，获取每一中转配关系在当前的边中的数量
			for (int i = 0; i < CNT_RELATIONS-1; i++) {
				assist = work.substr(0, work.find(','));
				work = work.substr(work.find(',') + 1);

				graph->_edgeCntRelations[id][id2][i] = stoi(assist);
				graph->_edgeCntRelations[id2][id][i] = stoi(assist);
			}
			//最后一个需要单独的处理
			graph->_edgeCntRelations[id][id2][CNT_RELATIONS - 1] = stoi(work);
			graph->_edgeCntRelations[id2][id][CNT_RELATIONS - 1] = stoi(work);
		}
		file.getline(buff, SIZE);/*获取下一个行*/
		work.assign(buff);
	}

	if (graph != NULL) {/*如果graph指针不为空，则将其放入result中*/
		result.push_back(graph);
	}
	file.close();

	return result;
}

//其实 pair(quG->vID[i], dbG->vID[j])就是一个候选pair candidate
//判断该候选pair是否满足feasibility rules
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

		for (i = 0; i < iS; ++i) { //每一个还没有match上的quG的节点	
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

	//获取整个装配体的包围盒的尺寸，box[0],box[1],box[2]是左下角点的坐标，也就是最小的坐标，box[3],box[4],box[5]是右上角点的坐标
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

	//求取整个装配体包围盒的尺寸
	double assemLen = assemBox[3] - assemBox[0], assemWid = assemBox[4] - assemBox[1], assemHi = assemBox[5] - assemBox[2];
	for (std::vector<double>&vec : vect) {

		//求取装配体中每个零件的质心在装配体包围盒中的相对位置
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

	//遍历当前点在图结构中每一个邻接点
	for (int next : graph[pres]) {
		//如果邻接点没有访问过，同时当前点在另一个图结构中有对应的点（quMATCHdb[next]为非负时有对应的点）
		if (vis[pres][next] == false&&quMATCHdb[next]>=0) {
			vis[pres][next] = vis[next][pres] = true;
			cnt++;
			//装配关系的相似度定义为：similarity=（两条边中相同的装配关系个数/（两条边中所有的装配关系的个数-两条边中相同的装配关系个数）
			//相同的装配关系中，较大值之和即为分母，较小值即为分子
			int lo = 0, hi = 0,presDb=quMATCHdb[pres],nextDb=quMATCHdb[next];
			for (int i = 0; i < CNT_RELATIONS; i++) {
				lo = lo + (edgeQu[pres][next][i] > edgeDb[presDb][nextDb][i] ? edgeQu[pres][next][i] : edgeDb[presDb][nextDb][i]);
				hi = hi + (edgeQu[pres][next][i] < edgeDb[presDb][nextDb][i] ? edgeQu[pres][next][i] : edgeDb[presDb][nextDb][i]);
			}
			//计算装配关系的相似度
			val = val + 1.0*hi / lo;
			//递归计算装配关系相似度
			val = val + getRelationDistance(next, cnt, graph, quMATCHdb, edgeQu, edgeDb, vis);
		}
	}

	return val;
}

long getShapeDistance(std::vector<int> vect1, std::vector<int> vect2)
{
	long result = 0;
	for (int i = 0; i < 512; i++) {
		result += abs(vect1[i] - vect2[i]);/*result为所有的柱子高度差值的绝对值的累加值*/
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

	shapeInf.open(fileName);/*打开存放装配体零件形状分布的文件*/
	if (!shapeInf) {
		std::cout << "Can not open file " << fileName << std::endl;/*无法打开，提示错误，退出*/
		exit(0);
	}

	shapeInf.getline(buff, SIZE);/*获取文件的一行，这行包含装配体中零件的个数*/
	work.assign(buff);
	while (shapeInf) {/*如果文件尚未到达结尾*/	
		//初始化shapeDis和box的大小为零件的个数，每一维的大小分别为512（形状分布的柱子个数）和6（零件包围盒两个角点的三维坐标）
		partCount = atoi(work.substr(1).c_str());
		shapeDis.resize(partCount);
		box.resize(partCount);
		for (int i = 0; i < partCount;i++) {
			shapeDis[i].resize(512);
			box[i].resize(6);
		}

		for (int i = 0; i < partCount; i++) {
			shapeInf.getline(buff, SIZE);/*获取文件的一行，这行是零件在装配体中的序号*/
			work.assign(buff);
			partNum = atoi(work.c_str());

			curs = 0;
			for (int j = 0; j < 16; j++) {/*每一个零件的形状分布的数据有16行*/
				shapeInf.getline(buff, SIZE);
				work.assign(buff);

				/*每一行的数据是32个，这里获取31个，最后一个数据单独处理。*/
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
			//包围盒坐标行以"box:"开头，需要跳过四个字符
			work.assign(buff+4);
			for (int k = 0; k < 5; k++) {
				pos = work.find(',');
				data = work.substr(0, pos);
				work = work.substr(pos + 1);
				box[partNum][curs++] = atof(data.c_str());
			}
			//最后一个坐标找不到','字符了，需要单独处理。
			box[partNum][curs] = atof(work.c_str());
		}

		//将当前的柱状图分布和包围盒坐标存储，并将临时的存放容器shapeDis和box清空
		_histogram.push_back(shapeDis);
		shapeDis.clear();
		normalization(box);
		_box.push_back(box);
		box.clear();

		shapeInf.getline(buff, SIZE);/*获取文件的一行，这行包含装配体中零件的个数*/
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
		//如果匹配的结果为非负数，则这个零件有和它相匹配的零件，计算这两个零件之间的形状相似度，空间位置相似度
			if (match._quMATCHdb[j] >= 0) {				
				accumShape += getShapeDistance(_histogram[qu][j], _histogram[db][match._quMATCHdb[j]]);
				simPos += getPosDistance(_box[qu][j],_box[db][match._quMATCHdb[j]]);
				i++;
			}
		}
		
		//计算装配关系的相似度，公式：similarity=（两条边中相同的装配关系个数/（两条边中所有的装配关系的个数-两条边中相同的装配关系个数）
		double simRelation = 0.0;		//result._similarity = 0.5*simPos + 0.5*simShape + 0.5*simRelation; ;
		//cnt是对匹配的边的计数，最后要求平均的边的装配关系相似度，即总的装配关系相似度
		int cnt = 0, size = graph[qu]->vAdjacencyEdge.size();
		std::vector<std::vector<bool>> vis(size, std::vector<bool>(size,false));
		//遍历qu图结构中每一个点，如果没有被访问过，而且在db图中有匹配的点（quMATCHdb[i] >= 0），则进行装配关系的相似度的计算
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
