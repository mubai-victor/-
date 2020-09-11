#include <VF2.h>
#include <vector>
#include <map>

struct EDGE
{
	int id2;
	int label;
	EDGE(int _id2, int _label)
	{
		id2 = _id2;
		label = _label;
	}
};


//邻接链表结构，不过是用vector来实现
struct GRAPH
{
	int graphID;
	std::vector<int> vID;
	std::vector<int> vLabel;


	std::vector<std::vector<EDGE> > vAdjacencyEdge;
	//外面的大vector< >，为每一个节点保存一个邻接表，一个图中有多少个节点，vAdjacencyEdge的size就是多少
	//vector<EDGE>存放EDGE[id2,label]组元，表示每个节点对应的兄弟节点id以及这两个节点间的边的label，
	//vector<EDGE>大小由每个节点的兄弟数量决定（这里所谓的兄弟，就是指“邻接点”）
	//因为可行pair(m,n)就是从当前状态M(s)的邻接点中寻找的，所以该结构能够加快搜索速度
};

//match结构，对应论文中提到的core_1 and core_2,
//whose dimensions correspond to the number of nodes in G1 and G2
struct MATCH
{
	//int *dbMATCHqu; //存储DB中的节点id和与之match的QU中的节点id
	//int *quMATCHdb; //存储QU中的节点id和与之match的DB中的节点id
	//使用map编程更方便，查找速度更快!
	std::map<int, int> dbMATCHqu;
	std::map<int, int> quMATCHdb;
};

std::vector<GRAPH *> ReadGraph(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	/*
	if (!fp)
	{
	printf("fp is NULL, file [%s] doesn't exist...\n", filename);
	return;
	}
	*/

	EDGE e(-1, -1);
	std::vector<EDGE> v;
	v.push_back(e);

	char mark[2];
	int id, label, id2;
	std::vector<GRAPH *> gSet;
	GRAPH * g = NULL;
	while (true)
	{
		fscanf(fp, "%s", mark);
		if (mark[0] == 't')
		{

			fscanf(fp, "%s%d", mark, &id);
			if (id == -1)
			{
				gSet.push_back(g);
				break;
			}
			else //if input not ending, then
			{
				if (g != NULL)
				{
					gSet.push_back(g);
				}
				g = new GRAPH;
				g->graphID = id;
			}
		}
		else if (mark[0] == 'v')
		{
			fscanf(fp, "%d%d", &id, &label);
			g->vID.push_back(id);/*如果标志位是v，则读取图中的点的id号以及边的label，存放到GRAPH中的vector中*/
			g->vLabel.push_back(label);

			g->vAdjacencyEdge.push_back(v);//为每个节点申请一个vAdjacencyEdge，其中v只是占用位置，没有任何用处！
		}
		else if (mark[0] == 'e')
		{
			fscanf(fp, "%d%d%d", &id, &id2, &label);

			e.id2 = id2; e.label = label;
			g->vAdjacencyEdge[id].push_back(e);//id->id2的边
			e.id2 = id; e.label = label;
			g->vAdjacencyEdge[id2].push_back(e);//id2->id的边
		}
	}

	fclose(fp);
	printf("graph number:%d\n", gSet.size());
	return gSet;

}

//其实 pair(quG->vID[i], dbG->vID[j])就是一个候选pair candidate
//判断该候选pair是否满足feasibility rules
bool FeasibilityRules(GRAPH *quG, GRAPH *dbG, MATCH match, int quG_vID, int dbG_vID)
{
	int quVid, dbVid, quGadjacencyEdgeSize, dbGadjacencyEdgeSize, i, j;
	bool flag = false;

	//首先，判断quG_vID和dbG_vID对应的label是否相同
	if (quG->vLabel[quG_vID] != dbG->vLabel[dbG_vID]) //如果两个点的label不同，则【一定不】满足feasibility rules
	{
		return false;
	}

	//其次，判断是不是每次match的第一个比较pair
	if (match.quMATCHdb.size() == 0) //如果是第一个比较pair
	{
		//只需要这两个点的label相同（已经判断成立了）即满足feasibility rules
		return true;
	}

	//最后（label相同，不是第一个pair【即，之前已经match了一部分节点】），那么只要下面条件成立就能满足最简单的feasibility rules：
	//1）quG_vID和dbG_vID与已经match的那些节点对中的【至少】一对(quVid,dbVid)分别相邻（quG_vID和dbG_vID分别是已经match的节点quVid和dbVid的“neighbor节点”）
	//2）如果存在多个相邻对(quVid,dbVid)，则必须要求【所有的】邻接边对( edge(quG_vID,quVid), edge(dbG_vID,dbVid) )的label一样
	for (std::map<int, int>::iterator iter = match.quMATCHdb.begin(); iter != match.quMATCHdb.end(); iter++) //遍历所有的已经match的节点对
	{
		quVid = iter->first;
		quGadjacencyEdgeSize = quG->vAdjacencyEdge[quVid].size();
		for (i = 1; i < quGadjacencyEdgeSize; i++) //从1开始依次扫描quVid的邻接点，第0个存的是(-1,-1)
		{
			//quG_vID是已经match的quG中的节点quVid的“第i个neighbor节点”
			if (quG->vAdjacencyEdge[quVid][i].id2 == quG_vID)
			{
				dbVid = iter->second;
				dbGadjacencyEdgeSize = dbG->vAdjacencyEdge[dbVid].size();
				for (j = 1; j < dbGadjacencyEdgeSize; j++) //从1开始依次扫描dbVid的邻接点，第0个存的是(-1,-1)
				{
					//同时，与quVid相match的节点dbVid在dbG中的“第j个neighbor节点”正好是dbG_vID
					if (dbG->vAdjacencyEdge[dbVid][j].id2 == dbG_vID)
					{
						//判断2）是否成立
						if (quG->vAdjacencyEdge[quVid][i].label != dbG->vAdjacencyEdge[dbVid][j].label)
						{
							//因为2）要求【所有的】label一样，只要有一个不一样，则返回false
							return false;
						}
						else
						{
							//标记：flag=true表示至少有一对满足1）的pair(dbVid,quVid)，同时满足了2）
							//因为有可能循环结束了，在所有的已经match的节点对里，找不到一个pair(dbVid,quVid)同时满足条件1）和2）
							flag = true;
						}
					}
				}
			}
		}
	}
	return flag;
}