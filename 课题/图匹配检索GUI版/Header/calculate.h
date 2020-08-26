#ifndef CALCULATE_H
#define CALCULATE_H
#include <VF2.h>

class Calculate {
private:
	std::vector<Graph*> _graph;
	GraphMatch _match;
	Retrival _retrival;
public:
	Calculate() {};
	void readGraphFile(char*fileName);
	void readShapeDistribution(char*fileName);
	std::vector<RetrivalRes> retrival(int ind);
	void clear() { _retrival.clear(); }
};


#endif