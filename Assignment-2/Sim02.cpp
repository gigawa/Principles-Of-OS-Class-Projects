#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include "ConfigData.h"
#include "MetaData.h"
#include "MemoryFunction.h"

using namespace std;

//stores all config file data
ConfigData config;

//stores each meta-data
vector<MetaData> meta;

int currProcess = 0;

void readMeta(string metafile);
void printMeta();
void executeProcess();
void *delay(void* time);

int main(int argc, char * argv[]) {
	try {
		//checks for correct amount of arguments
		if(argc == 2) {
			config.read(argv[1]);
			readMeta(config.filePath);
			//config.print();
			//printMeta();
			executeProcess();
		} else {
			throw invalid_argument("No config file specified.");
		}
	}
	//Handles errors - prints what is done so far and then exits
	catch(invalid_argument & e) {
		config.print();
		printMeta();
		cerr << endl << "ERROR: " << e.what() << endl;
		return 1;
	}

	return 0;
}

void executeProcess() {
	float start;
	float stop;
	int size = meta.size();

	start = clock();
	for(int i = 0; i < size; i++) {
		if(meta[i].code == 'I' || meta[i].code == 'O') {
			stop = clock();
			if(meta[i].code == 'I') {
				cout << (stop-start)/1000 << " - " << "start " << meta[i].desc << " input" << endl;
			}else {
				cout << (stop-start)/1000 << " - " << "start " << meta[i].desc << " output" << endl;
			}
			pthread_attr_t attr;
			pthread_attr_init (&attr);
			pthread_t tid;
			pthread_create(&tid, &attr, &delay, (void*) &meta[i].time);
			pthread_join(tid, NULL);
			stop = clock();
			if(meta[i].code == 'I') {
				cout << (stop-start)/1000 << " - " << "end " << meta[i].desc << " input" << endl;
			}else {
				cout << (stop-start)/1000 << " - " << "end " << meta[i].desc << " output" << endl;
			}
		}else if(meta[i].code == 'P' || meta[i].code == 'M'){
			stop = clock();
			if(meta[i].desc == "run") {
				cout << (stop-start)/1000 << " - " << "start processing action" << endl;
				usleep(meta[i].time);
				stop = clock();
				cout << (stop-start)/1000 << " - " << "end processing action" << endl;
			} else if(meta[i].desc == "block") {
				cout << (stop-start)/1000 << " - " << "start memory blocking" << endl;
				usleep(meta[i].time);
				stop = clock();
				cout << (stop-start)/1000 << " - " << "end memory blocking" << endl;
			} else if(meta[i].desc == "allocate") {
				cout << (stop-start)/1000 << " - " << "allocating memory" << endl;
				usleep(meta[i].time);
				stop = clock();
				cout << (stop-start)/1000 << " - " << "memory allocated at " << endl;
			}
		}else {
			stop = clock();
			if(meta[i].desc == "start") {
				if(meta[i].code == 'A') {
					cout << (stop-start)/1000 << " - " << "OS: starting process " << currProcess<< endl;
				} else {
					cout << (stop-start)/1000 << " - " << "Simulator Program Starting" << endl;
				}
			} else {
				if(meta[i].code == 'A') {
					cout << (stop-start)/1000 << " - " << "OS: removing process " << currProcess<< endl;
				} else {
					cout << (stop-start)/1000 << " - " << "Simulator Program Ending" << endl;
				}
			}
		}
	}
}

void *delay(void * time) {
	int * delayTime;
	delayTime = (int*)time;
	usleep(*delayTime);
	pthread_exit(NULL);
}

//reads in the meta-data file
void readMeta(string metafile) {
	string line;

	//finds the last period marking the file extension
	size_t extension = metafile.rfind(".");
	line = metafile.substr(extension+1);

	//checks for correct file extension and if file can be opened
	if(line != "mdf") {
		throw invalid_argument("Incorrect file extension for Meta-Data file");
	}
	fstream fin(metafile);
	if(!fin) {
		throw invalid_argument("Meta-Data file cannot be opened");
	}

	int configCycle;

	//resets line
	line = "";

	getline(fin, line);

	//tempMeta used to temporarily store data until added to vector
	MetaData tempMeta;

	//checks for start of meta-data file
	if(line == "Start Program Meta-Data Code:") {
		//checks for end
		while(!(tempMeta.desc == "end" && tempMeta.code == 'S')) {
			fin >> tempMeta.code;

			//checks if the meta-data code has been read in
			if(tempMeta.code != 'S' && tempMeta.code != 'A' && tempMeta.code != 'P' && tempMeta.code != 'I' && tempMeta.code != 'O' && tempMeta.code != 'M') {
				fin >> tempMeta.code;
				//checks if meta-data code exists
				if(tempMeta.code != 'S' && tempMeta.code != 'A' && tempMeta.code != 'P' && tempMeta.code != 'I' && tempMeta.code != 'O' && tempMeta.code != 'M') {
					fin.close();
					throw invalid_argument("Invalid or missing meta-data code");
				}
				//moves to parenthesis indicating descriptor
				getline(fin, line, '(');
			} else {
				char tempChar;
				//removes white space and removes parenthesis
				fin >> ws;
				fin >> tempChar;
			}
			//removes white space and moves through end of parenthesis
			fin >> ws;
			getline(fin, tempMeta.desc, ')');
			if(!(fin >> tempMeta.cycles))
			{
				fin.close();
				throw invalid_argument("Missing number of cycles in meta-data");
			}
			if(tempMeta.cycles < 0) {
				fin.close();
				throw invalid_argument("Invalid number of cycles in meta-data");
			}

			//determines what config cycle times to use
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
			}else if(tempMeta.desc == "start") {
				configCycle = 0;
			}else if(tempMeta.desc == "end"{
				configCycle = 0;
			}else
				fin.close();
				throw invalid_argument("Invalid or missing descriptor in meta-data");
			}
			tempMeta.time = (tempMeta.cycles * configCycle);
			meta.push_back(tempMeta);
		}
	//checks for empty file if start is not encountered
	} else if(line.empty()) {
		fin.close();
		throw invalid_argument("The Meta-Data file is empty");
	}
	fin.close();
}

//prints meta-data
void printMeta() {
	int size = meta.size();
	//logs to the monitor
	if(config.log == "Log to Both" || config.log == "Log to Monitor") {
		cout << "Meta-Data Metrics" << endl;
		for(int i = 0; i < size; i++) {
			//ignores start and end when printing
			if(meta[i].code != 'S' && meta[i].code != 'A') {
				cout << meta[i].code << "(" << meta[i].desc << ")" << meta[i].cycles << " - " << meta[i].time <<endl;
			}
		}
	}

	//logs to the file specified
	if(config.log == "Log to Both" || config.log == "Log to File") {
		ofstream fout;
		//opens specified file and appends to the end
		fout.open(config.logFilePath, ofstream::out | ofstream::app);
		fout << "Meta-Data Metrics" << endl;
		for(int i = 0; i < size; i++) {
			//ignores start and end when printing
			if(meta[i].code != 'S' && meta[i].code != 'A') {
				fout << meta[i].code << "(" << meta[i].desc << ")" << meta[i].cycles << " - " << meta[i].time <<endl;
			}
		}
	}
}
