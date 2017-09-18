#ifndef CONFIG_DATA_
#define CONFIG_DATA_

#include<string>

using namespace std;

class ConfigData {
public:
	void read(char * filename);
	void print();

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
	int systemMemory;
	int memoryBlock;
	int printerQuantity;
	int hardDriveQuantity;
	string cpuSchedule;
	string log;
	string logFilePath;
};

#endif
