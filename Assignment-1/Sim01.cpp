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
		return 1;
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
		while(line != "End") {
			fin >> line;
			if(line == "Version/Phase:") {
				fin >> config.version;
			} else if(line == "File") {
				getline(fin, line, ':');
				fin >> config.filePath;
			} else if(line == "Processor") {
				getline(fin, line, ':');
				fin >> config.processor;
			} else if(line == "Monitor") {
				getline(fin, line, ':');
				fin >> config.monitor;
			} else if(line == "Hard") {
				getline(fin, line, ':');
				fin >> config.hardDrive;
			} else if(line == "Printer") {
				getline(fin, line, ':');
				fin >> config.printer;
			} else if(line == "Keyboard") {
				getline(fin, line, ':');
				fin >> config.keyboard;
			} else if(line == "Memory") {
				getline(fin, line, ':');
				fin >> config.memory;
			} else if(line == "Mouse") {
				getline(fin, line, ':');
				fin >> config.mouse;
			} else if(line == "Speaker") {
				getline(fin, line, ':');
				fin >> config.speaker;
			} else if(line == "Log:") {
				fin >> ws;
				getline(fin, config.log);
			} else if(line == "Log") {
				getline(fin, line, ':');
				fin >> config.logFilePath;
			}
		}
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
	getline(fin, line);
	MetaData tempMeta;
		if(line == "Start Program Meta-Data Code:") {
			while(!(tempMeta.desc == "end" && tempMeta.code == 'S')) {
				meta.resize(currMeta);
				fin >> tempMeta.code;
				if(tempMeta.code != 'S' && tempMeta.code != 'A' && tempMeta.code != 'P' && tempMeta.code != 'I' && tempMeta.code != 'O' && tempMeta.code != 'M') {
					fin >> tempMeta.code;
					getline(fin, line, '(');
				} else {
					char tempChar;
					fin >> ws;
					fin >> tempChar;
				}
				fin >> ws;
				getline(fin, tempMeta.desc, ')');
				fin >> tempMeta.cycles;
				if(tempMeta.desc == "run") {
					configCycle = config.processor;
				}else if(tempMeta.desc == "monitor") {
					configCycle = config.monitor;
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
	if(config.log == "Log to Both" || config.log == "Log to Monitor") {
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

	if(config.log == "Log to Both" || config.log == "Log to File") {
		ofstream fout;
		fout.open(config.logFilePath);
		fout << "Configuration File Data" << endl;
		fout << "Processor = " << config.processor << " ms/cycle" << endl;
		fout << "Monitor = " << config.monitor << " ms/cycle" << endl;
		fout << "Hard Drive = " << config.hardDrive << " ms/cycle" << endl;
		fout << "Printer = " << config.printer << " ms/cycle" << endl;
		fout << "Keyboard = " << config.keyboard << " ms/cycle" << endl;
		fout << "Memory = " << config.memory << " ms/cycle" << endl;
		fout << "Mouse = " << config.mouse << " ms/cycle" << endl;
		fout << "Speaker = " << config.speaker << " ms/cycle" << endl << endl;
	}
}

void printMeta() {
	if(config.log == "Log to Both" || config.log == "Log to Monitor") {
		cout << "Meta-Data Metrics" << endl;
		for(int i = 0; i < metaLength; i++) {
			if(meta[i].code != 'S' && meta[i].code != 'A') {
				cout << meta[i].code << "(" << meta[i].desc << ")" << meta[i].cycles << " - " << meta[i].time <<endl;
			}
		}
	}

	if(config.log == "Log to Both" || config.log == "Log to File") {
		ofstream fout;
		fout.open(config.logFilePath, ofstream::out | ofstream::app);
		fout << "Meta-Data Metrics" << endl;
		for(int i = 0; i < metaLength; i++) {
			if(meta[i].code != 'S' && meta[i].code != 'A') {
				fout << meta[i].code << "(" << meta[i].desc << ")" << meta[i].cycles << " - " << meta[i].time <<endl;
			}
		}
	}
}
