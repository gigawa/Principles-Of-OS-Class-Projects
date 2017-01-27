#ifndef CONFIG_DATA_
#define CONFIG_DATA_

#include<string>

using namespace std;

class ConfigData {
public:
	float version;
	string filePath;
	int processor;
	int monitor;
	int hardDrive;
	int printer;
	int keyboard;
	int memory;
	int mouse;
	int speaker;
	string log;
	string logFilePath;
};

#endif
