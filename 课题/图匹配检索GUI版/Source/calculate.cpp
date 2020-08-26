// ����ͼ���صļ���.cpp : �������̨Ӧ�ó������ڵ㡣

//#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "VF2.h"
#include "time.h"
#include "set"                                                              
#include <algorithm>
#include <calculate.h>

#define SIZE 400

void Calculate::readGraphFile(char*fileName) {
	_graph = _match.readGraph(fileName);
}
void Calculate::readShapeDistribution(char*fileName) {
	_retrival.getShapeDistribution(fileName);
}
std::vector<RetrivalRes> Calculate::retrival(int select) {
	std::vector<GraphMatch> maps;
	Match result;
	std::map<int, int> sort1;/*��һ��intΪͼ����ţ��ڶ���Ϊͼ�ĳ��ֵĴ���*/
	std::vector<std::map<int, std::string>> map;
	std::list<Entry> sort2;
	Entry entry;
	char buff[SIZE];
	std::string work, temp;
	std::map<int, std::string> elem;

	Graph*p = _graph[select];
	sort1.clear();
	sort2.clear();

	for (std::map<unsigned long long, int>::iterator iter = p->_subGraph.begin(); iter != p->_subGraph.end(); iter++) {/*��ȡ��ǰͼ�ṹ�е�ÿһ����ͼ*/
		for (Entry entry : Graph::_list[iter->first]) {/*����Graph������_list�а�����ǰͼ�ĵ�ǰ��ͼ�ṹ��list���е�ÿһ��ͼ��*/
			if (entry._tag != select) {/*����������Ĳ��������ͼ����Ҫ��¼������ǰ��ͼ������*/
				if (sort1.count(entry._tag) == 0) {/*�����ǰͼ�ṹ�ı����δ��sort��*/
					sort1[entry._tag] = iter->second < entry._count ? iter->second : entry._count;
				}
				else {
					sort1[entry._tag] += (iter->second < entry._count ? iter->second : entry._count);
				}
			}
		}
	}

	for (std::map<int, int>::iterator iter = sort1.begin(); iter != sort1.end(); iter++) {
		entry._count = iter->second;
		entry._tag = iter->first;

		sort2.push_back(entry);
	}
	sort2.sort();

	for (Entry entry : sort2) {
		_match.setQuery(select);
		_match.setDatabase(entry._tag);
		_match.subGraphMatch(_graph[select], _graph[entry._tag]);

		maps.push_back(_match);
		_match.clear();
	}

	_retrival.calculate(maps,_graph);
	return _retrival.getResult();
}