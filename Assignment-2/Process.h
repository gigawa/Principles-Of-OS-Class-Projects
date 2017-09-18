#ifndef PROCESS
#define PROCESS

#include <vector>
#include <string>
#include "MetaData.h"

using namespace std;

class Process {
public:
	vector<MetaData> meta;
	string state;
};

#endif
