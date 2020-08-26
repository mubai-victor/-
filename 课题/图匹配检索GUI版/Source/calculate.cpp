// 基于图搜素的检索.cpp : 定义控制台应用程序的入口点。

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
	std::map<int, int> sort1;/*第一个int为图的序号，第二个为图的出现的次数*/
	std::vector<std::map<int, std::string>> map;
	std::list<Entry> sort2;
	Entry entry;
	char buff[SIZE];
	std::string work, temp;
	std::map<int, std::string> elem;

	Graph*p = _graph[select];
	sort1.clear();
	sort2.clear();

	for (std::map<unsigned long long, int>::iterator iter = p->_subGraph.begin(); iter != p->_subGraph.end(); iter++) {/*获取当前图结构中的每一个子图*/
		for (Entry entry : Graph::_list[iter->first]) {/*遍历Graph类型中_list中包含当前图的当前子图结构的list的中的每一个图。*/
			if (entry._tag != select) {/*如果遍历到的不是输入的图，则要记录包含当前子图的数量*/
				if (sort1.count(entry._tag) == 0) {/*如果当前图结构的标号尚未在sort中*/
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