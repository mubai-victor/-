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


//�ڽ�����ṹ����������vector��ʵ��
struct GRAPH
{
	int graphID;
	std::vector<int> vID;
	std::vector<int> vLabel;


	std::vector<std::vector<EDGE> > vAdjacencyEdge;
	//����Ĵ�vector< >��Ϊÿһ���ڵ㱣��һ���ڽӱ�һ��ͼ���ж��ٸ��ڵ㣬vAdjacencyEdge��size���Ƕ���
	//vector<EDGE>���EDGE[id2,label]��Ԫ����ʾÿ���ڵ��Ӧ���ֵܽڵ�id�Լ��������ڵ��ıߵ�label��
	//vector<EDGE>��С��ÿ���ڵ���ֵ�����������������ν���ֵܣ�����ָ���ڽӵ㡱��
	//��Ϊ����pair(m,n)���Ǵӵ�ǰ״̬M(s)���ڽӵ���Ѱ�ҵģ����Ըýṹ�ܹ��ӿ������ٶ�
};

//match�ṹ����Ӧ�������ᵽ��core_1 and core_2,
//whose dimensions correspond to the number of nodes in G1 and G2
struct MATCH
{
	//int *dbMATCHqu; //�洢DB�еĽڵ�id����֮match��QU�еĽڵ�id
	//int *quMATCHdb; //�洢QU�еĽڵ�id����֮match��DB�еĽڵ�id
	//ʹ��map��̸����㣬�����ٶȸ���!
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
			g->vID.push_back(id);/*�����־λ��v�����ȡͼ�еĵ��id���Լ��ߵ�label����ŵ�GRAPH�е�vector��*/
			g->vLabel.push_back(label);

			g->vAdjacencyEdge.push_back(v);//Ϊÿ���ڵ�����һ��vAdjacencyEdge������vֻ��ռ��λ�ã�û���κ��ô���
		}
		else if (mark[0] == 'e')
		{
			fscanf(fp, "%d%d%d", &id, &id2, &label);

			e.id2 = id2; e.label = label;
			g->vAdjacencyEdge[id].push_back(e);//id->id2�ı�
			e.id2 = id; e.label = label;
			g->vAdjacencyEdge[id2].push_back(e);//id2->id�ı�
		}
	}

	fclose(fp);
	printf("graph number:%d\n", gSet.size());
	return gSet;

}

//��ʵ pair(quG->vID[i], dbG->vID[j])����һ����ѡpair candidate
//�жϸú�ѡpair�Ƿ�����feasibility rules
bool FeasibilityRules(GRAPH *quG, GRAPH *dbG, MATCH match, int quG_vID, int dbG_vID)
{
	int quVid, dbVid, quGadjacencyEdgeSize, dbGadjacencyEdgeSize, i, j;
	bool flag = false;

	//���ȣ��ж�quG_vID��dbG_vID��Ӧ��label�Ƿ���ͬ
	if (quG->vLabel[quG_vID] != dbG->vLabel[dbG_vID]) //����������label��ͬ����һ����������feasibility rules
	{
		return false;
	}

	//��Σ��ж��ǲ���ÿ��match�ĵ�һ���Ƚ�pair
	if (match.quMATCHdb.size() == 0) //����ǵ�һ���Ƚ�pair
	{
		//ֻ��Ҫ���������label��ͬ���Ѿ��жϳ����ˣ�������feasibility rules
		return true;
	}

	//���label��ͬ�����ǵ�һ��pair������֮ǰ�Ѿ�match��һ���ֽڵ㡿������ôֻҪ����������������������򵥵�feasibility rules��
	//1��quG_vID��dbG_vID���Ѿ�match����Щ�ڵ���еġ����١�һ��(quVid,dbVid)�ֱ����ڣ�quG_vID��dbG_vID�ֱ����Ѿ�match�Ľڵ�quVid��dbVid�ġ�neighbor�ڵ㡱��
	//2��������ڶ�����ڶ�(quVid,dbVid)�������Ҫ�����еġ��ڽӱ߶�( edge(quG_vID,quVid), edge(dbG_vID,dbVid) )��labelһ��
	for (std::map<int, int>::iterator iter = match.quMATCHdb.begin(); iter != match.quMATCHdb.end(); iter++) //�������е��Ѿ�match�Ľڵ��
	{
		quVid = iter->first;
		quGadjacencyEdgeSize = quG->vAdjacencyEdge[quVid].size();
		for (i = 1; i < quGadjacencyEdgeSize; i++) //��1��ʼ����ɨ��quVid���ڽӵ㣬��0�������(-1,-1)
		{
			//quG_vID���Ѿ�match��quG�еĽڵ�quVid�ġ���i��neighbor�ڵ㡱
			if (quG->vAdjacencyEdge[quVid][i].id2 == quG_vID)
			{
				dbVid = iter->second;
				dbGadjacencyEdgeSize = dbG->vAdjacencyEdge[dbVid].size();
				for (j = 1; j < dbGadjacencyEdgeSize; j++) //��1��ʼ����ɨ��dbVid���ڽӵ㣬��0�������(-1,-1)
				{
					//ͬʱ����quVid��match�Ľڵ�dbVid��dbG�еġ���j��neighbor�ڵ㡱������dbG_vID
					if (dbG->vAdjacencyEdge[dbVid][j].id2 == dbG_vID)
					{
						//�ж�2���Ƿ����
						if (quG->vAdjacencyEdge[quVid][i].label != dbG->vAdjacencyEdge[dbVid][j].label)
						{
							//��Ϊ2��Ҫ�����еġ�labelһ����ֻҪ��һ����һ�����򷵻�false
							return false;
						}
						else
						{
							//��ǣ�flag=true��ʾ������һ������1����pair(dbVid,quVid)��ͬʱ������2��
							//��Ϊ�п���ѭ�������ˣ������е��Ѿ�match�Ľڵ����Ҳ���һ��pair(dbVid,quVid)ͬʱ��������1����2��
							flag = true;
						}
					}
				}
			}
		}
	}
	return flag;
}