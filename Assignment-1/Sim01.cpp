#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ConfigData.h"
#include "MetaData.h"

using namespace std;

ConfigData config;
vector<MetaData> meta;

void readConfig(char * argv);
void readMeta(string metafile);
void printConfig();
void printMeta();
int metaLength;

int main(int argc, char * argv[]) {
	try {
		if(argc > 1) {
			readConfig(argv[1]);
			readMeta(config.filePath);
			printConfig();
			printMeta();
		} else {
			throw invalid_argument("No config file specified.");
		}
	}
	catch(invalid_argument & e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}

void readConfig(char * argv) {
	string line;
	fstream fin(argv);
	if(!fin) {
		throw invalid_argument("Config file cannot be opened");
	}
	getline(fin, line);
	if(line == "Start Simulator Configuration File") {
		getline(fin, line, ':');
		fin >> config.version;
		getline(fin, line, ':');
		fin >> config.filePath;
		getline(fin, line, ':');
		fin >> config.processor;
		getline(fin, line, ':');
		fin >> config.monitor;
		getline(fin, line, ':');
		fin >> config.hardDrive;
		getline(fin, line, ':');
		fin >> config.printer;
		getline(fin, line, ':');
		fin >> config.keyboard;
		getline(fin, line, ':');
		fin >> config.memory;
		getline(fin, line, ':');
		fin >> config.mouse;
		getline(fin, line, ':');
		fin >> config.speaker;
		getline(fin, line, ':');
		fin >> config.log;
		getline(fin, line, ':');
		fin >> config.logFilePath;
	}
	fin.close();
	
}

void readMeta(string metafile) {
	fstream fin(metafile);
	if(!fin) {
		throw invalid_argument("Meta-Data file cannot be opened");
	}
	string line;
	int currMeta = 0;
	int configCycle;
	char metaCode;
	getline(fin, line);
	MetaData tempMeta;
		if(line == "Start Program Meta-Data Code:") {
			while(!(tempMeta.desc == "end" && tempMeta.code == "S")) {
				meta.resize(currMeta);
				fin >> metaCode;
				cout << metaCode << endl;
				if(metaCode != 'S' && metaCode != 'A' && metaCode != 'P' && metaCode != 'I' && metaCode != 'O' && metaCode != 'M') {
					fin >> ws;
					getline(fin, tempMeta.code, '(');
				} else {
					tempMeta.code = metaCode;
					fin >> metaCode;
				}
				getline(fin, tempMeta.desc, ')');
				fin >> tempMeta.cycles;
				//getline(fin, line, ';');
				//tempMeta.cycles = atoi(line.c_str());
				if(tempMeta.desc == "run") {
					configCycle = config.processor;
				}else if(tempMeta.desc == "monitor") {
					configCycle = config.processor;
				}else if(tempMeta.desc == "hard drive") {
					configCycle = config.hardDrive;
				}else if(tempMeta.desc == "printer") {
					configCycle = config.printer;
				}else if(tempMeta.desc == "keyboard") {
					configCycle = config.keyboard;
				}else if(tempMeta.desc == "block" || tempMeta.desc == "allocate") {
					configCycle = config.memory;
				}else if(tempMeta.desc == "mouse") {
					configCycle = config.mouse;
				}else if(tempMeta.desc == "speaker") {
					configCycle = config.speaker;
				}
				tempMeta.time = (tempMeta.cycles * configCycle);
				meta.push_back(tempMeta);
				currMeta++;
			}
		}
	metaLength = currMeta;
	fin.close();
}

void printConfig() {
	cout << "Configuration File Data" << endl;
	cout << "Processor = " << config.processor << " ms/cycle" << endl;
	cout << "Monitor = " << config.monitor << " ms/cycle" << endl;
	cout << "Hard Drive = " << config.hardDrive << " ms/cycle" << endl;
	cout << "Printer = " << config.printer << " ms/cycle" << endl;
	cout << "Keyboard = " << config.keyboard << " ms/cycle" << endl;
	cout << "Memory = " << config.memory << " ms/cycle" << endl;
	cout << "Mouse = " << config.mouse << " ms/cycle" << endl;
	cout << "Speaker = " << config.speaker << " ms/cycle" << endl << endl;
}

void printMeta() {
	cout << "Meta-Data Metrics" << endl;
	for(int i = 0; i < metaLength; i++) {
		if(meta[i].code != "S" && meta[i].code != "A") {
			cout << meta[i].code << "(" << meta[i].desc << ")" << meta[i].cycles << " - " << meta[i].time <<endl;
		}
	}
}
