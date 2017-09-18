#ifndef META_DATA_
#define META_DATA_

#include<string>

using namespace std;

class MetaData {
public:
	MetaData * read(char * filename);
	void print(bool monitor, bool file);

	char code;
	string desc;
	int cycles;
	int time;
};

#endif
