#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <iomanip>
#include "ConfigData.h"
#include "MetaData.h"
#include "Process.h"

using namespace std;

enum PCBStates {startState, readyState, runningState, waitingState, exitState};

struct PCB {
	PCBStates state;
};

//stores all config file data
ConfigData config;

//stores each process
vector<Process> process;
vector<PCB> pcb;

//clock starting point
double clockStart;
int currProcess = -1;

//semaphore object
sem_t semaphore;

void readMeta(string metafile);
void executeProcesses();
void *threadDelay(void* time);
void delay(float time);
unsigned int allocateMemory(int totMem);

int main(int argc, char * argv[]) {
	try {
		//checks for correct amount of arguments
		if(argc == 2) {
			config.read(argv[1]);
			cout.setf(ios::fixed, ios::floatfield);
			readMeta(config.filePath);
			executeProcesses();
		} else {
			throw invalid_argument("No config file specified.");
		}
	}
	//Handles errors - prints what is done so far and then exits
	catch(invalid_argument & e) {
		if(config.log == "Log to Both" || config.log == "Log to File") {
			ofstream fout;
			fout.open(config.logFilePath, ofstream::out | ofstream::app);
			cout << "Logged to: " << config.logFilePath << endl;
			fout << endl << "ERROR: " << e.what() << endl;
		}
		cerr << endl << "ERROR: " << e.what() << endl;
		return 1;
	}

	return 0;
}

//executes metadata read in
void executeProcesses() {
	//mutex object
	pthread_mutex_t mutex;
	
	ofstream fout;
	fout.setf(ios::fixed, ios::floatfield);
	if(config.log == "Log to Both" || config.log == "Log to File") {
		fout.open(config.logFilePath, ofstream::out | ofstream::app);
	}
	
	//stop time for clock
	double stop;
	int processSize = process.size();

	//bools indicating the state of the OS and processes
	bool runningProcess = false;
	bool runningOS = false;

	//the difference between the starting time and current time of the clock
	double clockTime;

	int printer = -1;
	int hdd = -1;

	//for loop to run each process
	for(int j = 0; j < processSize; j++) {
		int metaSize = process[j].meta.size();
		currProcess = j;

		//for loop to run each meta in the process
		for(int i = 0; i < metaSize; i++) {
			if(process[j].meta[i].code == 'I' || process[j].meta[i].code == 'O') {
				stop = clock();

				//locks mutex
				pthread_mutex_lock(&mutex);
				pcb[currProcess].state = runningState;

				//sets clock time and prints it to file/screen for input/output start time
				if(process[j].meta[i].code == 'I') {
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(process[j].meta[i].desc == "printer") {
						printer += 1;
						if(printer > config.printerQuantity-1) {
							printer = 0;
						}
					}else if(process[j].meta[i].desc == "hard drive") {
						hdd += 1;
						if(hdd > config.hardDriveQuantity-1) {
							hdd = 0;
						}
					}

					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						if(process[j].meta[i].desc == "printer") {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " input on PRNTR " << dec << printer << endl;
						}else if(process[j].meta[i].desc == "hard drive") {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " input on HDD " << dec << hdd << endl;
						}else {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " input" << endl;
						}
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						if(process[j].meta[i].desc == "printer") {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " input on PRNTR " << dec << printer << endl;
						}else if(process[j].meta[i].desc == "hard drive") {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " input on HDD " << dec << hdd << endl;
						}else {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " input" << endl;
						}
					}
					
				}else {
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(process[j].meta[i].desc == "printer") {
						printer += 1;
						if(printer > config.printerQuantity-1) {
							printer = 0;
						}
					}else if(process[j].meta[i].desc == "hard drive") {
						hdd += 1;
						if(hdd > config.hardDriveQuantity-1) {
							hdd = 0;
						}
					}

					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						if(process[j].meta[i].desc == "printer") {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " output on PRNTR " << dec << printer << endl;
						}else if(process[j].meta[i].desc == "hard drive") {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " output on HDD " << dec << hdd << endl;
						}else {
							cout << clockTime << " - " << "start " << process[j].meta[i].desc << " output" << endl;
						}
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						if(process[j].meta[i].desc == "printer") {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " output on PRNTR " << dec << printer << endl;
						}else if(process[j].meta[i].desc == "hard drive") {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " output on HDD " << dec << hdd << endl;
						}else {
							fout << clockTime << " - " << "start " << process[j].meta[i].desc << " output" << endl;
						}
					}
				}

				//initialize and create thread
				sem_init(&semaphore, 0, 1);
				pthread_attr_t attr;
				pthread_attr_init (&attr);
				pthread_t tid;
				pthread_create(&tid, &attr, &threadDelay, (void*) &process[j].meta[i].time);
				pthread_join(tid, NULL);
				sem_destroy(&semaphore);
				stop = clock();

				//sets clock time and prints it to file/screen for input/output end time
				if(process[j].meta[i].code == 'I') {
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "end " << process[j].meta[i].desc << " input" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "end " << process[j].meta[i].desc << " input" << endl;
					}
					
				}else {
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "end " << process[j].meta[i].desc << " output" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "end " << process[j].meta[i].desc << " output" << endl;
					}
				}
			}else if(process[j].meta[i].code == 'P' || process[j].meta[i].code == 'M'){
				stop = clock();
				pcb[currProcess].state = runningState;

				//prints clock time to file/screen for processing action start/end time
				if(process[j].meta[i].desc == "run") {
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "start processing action" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "start processing action" << endl;
					}
					delay(process[j].meta[i].time);
					stop = clock();
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "end processing action" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "end processing action" << endl;
					}
				} else if(process[j].meta[i].desc == "block") {

					//prints clock time to file/screen for memory blocking start/end time
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "start memory blocking" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "start memory blocking" << endl;
					}
					delay(process[j].meta[i].time);
					stop = clock();
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "end memory blocking" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "end memory blocking" << endl;
					}
				} else if(process[j].meta[i].desc == "allocate") {
					fout << showbase << internal << setfill('0');

					//prints clock time to file/screen for allocating memory start/end time
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "allocating memory" << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "allocating memory" << endl;
					}

					//calls memory allocation
					int allocation = allocateMemory(config.systemMemory);
					delay(process[j].meta[i].time);
					stop = clock();
					clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "memory allocated at " << hex << setfill('0') << "0x" << setw(8) << allocation << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "memory allocated at " << hex << setfill('0') << "0x" << setw(8) << allocation << endl;
					}
					
				}
			}else {
				stop = clock();
				clockTime = ((stop-clockStart)/(double)CLOCKS_PER_SEC);
				if(process[j].meta[i].desc == "start") {

					//prints clock time to file/screen for allocating memory start/end time
					if(process[j].meta[i].code == 'A') {
						if(runningProcess) {
							//cant start process while one is already running
							throw invalid_argument("Cannot start process while process is already running");
						}
						runningProcess = true;
						if(config.log == "Log to Both" || config.log == "Log to Monitor") {
							cout << clockTime << " - " << "OS: starting process " << j + 1 << endl;
						}
						if(config.log == "Log to Both" || config.log == "Log to File") {
							fout << clockTime << " - " << "OS: starting process " << j + 1 << endl;
						}
					} else {
						if(runningOS) {
							//cant start OS while it is already running
							throw invalid_argument("OS is already running");
						}
						runningOS = true;
						if(config.log == "Log to Both" || config.log == "Log to Monitor") {
							cout << clockTime << " - " << "Simulator Program Starting" << endl;
						}
						if(config.log == "Log to Both" || config.log == "Log to File") {
							fout << clockTime << " - " << "Simulator Program Starting" << endl;
						}
						pcb[currProcess].state = readyState;
					}
				} else {
					if(process[j].meta[i].code == 'A') {

						//prints clock time to file/screen for removing process time
						runningProcess = false;
						if(config.log == "Log to Both" || config.log == "Log to Monitor") {
							cout << clockTime << " - " << "OS: removing process " << j + 1 << endl;
						}
						if(config.log == "Log to Both" || config.log == "Log to File") {
							fout << clockTime << " - " << "OS: removing process " << j + 1 << endl;
						}
						pcb[currProcess].state = exitState;
					} else {

						//prints clock time to file/screen for ending simulator time
						runningOS = false;
						if(config.log == "Log to Both" || config.log == "Log to Monitor") {
							cout << clockTime << " - " << "Simulator Program Ending" << endl;
						}
						if(config.log == "Log to Both" || config.log == "Log to File") {
							fout << clockTime << " - " << "Simulator Program Ending" << endl;
							cout << "Logged to: " << config.logFilePath << endl;
						}
					}
				}
			}
			//unlocks mutex
			pthread_mutex_unlock(&mutex);
		}
	}
	fout.close();
}

//delay function called by thread
void *threadDelay(void * time) {
	pcb[currProcess].state = waitingState;
	sem_wait(&semaphore);
	int * delayTime;
	delayTime = (int*)time;
	clock_t start;
	clock_t stop;
	double timer = 0;

	start = clock();
	while(timer < ((float)(*delayTime)/1000)) {
		stop = clock();
		timer = (stop-start)/(double) CLOCKS_PER_SEC;
		//cout << (stop-start)/(double) CLOCKS_PER_SEC << endl;
	}
	pthread_exit(NULL);
	sem_post(&semaphore);
}

//delay function to simulate running
void delay(float time) {
	pcb[currProcess].state = waitingState;

	float start;
	float stop;
	double timer = 0;

	start = clock();
	while(timer < (time/1000)) {
		stop = clock();
		timer = (stop-start)/(double) CLOCKS_PER_SEC;
		//cout << (stop-start)/(double) CLOCKS_PER_SEC << endl;
	}
	//cout << (stop-start)/(double) CLOCKS_PER_SEC << endl;
}

//reads in the meta-data file
void readMeta(string metafile) {
	string line;
	ofstream fout;
	fout.setf(ios::fixed, ios::floatfield);
	bool runningOS = false;

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

	if(config.log == "Log to Both" || config.log == "Log to File") {
		fout.open(config.logFilePath);
	}

	int configCycle;

	//resets line
	line = "";

	getline(fin, line);

	//tempMeta used to temporarily store data until added to vector
	MetaData tempMeta;
	Process tempProcess;
	PCB tempPCB;

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
			}else if(tempMeta.desc == "start" || tempMeta.desc == "end") {
				configCycle = 0;

				//creates new process and PCB from meta data
				if(tempMeta.code == 'A' && tempMeta.desc == "start") {
					process.push_back(tempProcess);
					pcb.push_back(tempPCB);
					currProcess++;
					pcb[currProcess].state = startState;
					double stop = clock();
					double clockTime = (stop-clockStart)/(double)CLOCKS_PER_SEC;
					if(currProcess > 0) {
						pcb[currProcess-1].state = readyState;
					}

					//prints clock time to file/screen for preparing process time
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "OS: preparing process " << currProcess + 1 << endl;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "OS: preparing process " << currProcess + 1 << endl;
					}
				} else if(tempMeta.code == 'S' && tempMeta.desc == "start") {
					//clock starts with simulator start
					clockStart = clock();
					double stop = clock();
					double clockTime = (stop-clockStart)/(double)CLOCKS_PER_SEC;

					if(runningOS) {
						throw invalid_argument("OS is already running");
					}

					//prints clock time to file/screen for simulator program starting time
					if(config.log == "Log to Both" || config.log == "Log to Monitor") {
						cout << clockTime << " - " << "Simulator Program Starting" << endl;
						runningOS = true;
					}
					if(config.log == "Log to Both" || config.log == "Log to File") {
						fout << clockTime << " - " << "Simulator Program Starting" << endl;
						runningOS = true;
					}
				}
			}else {
				fin.close();
				throw invalid_argument("Invalid or missing descriptor in meta-data");
			}
			tempMeta.time = (tempMeta.cycles * configCycle);

			//adds meta to end of process
			if(process.size() > 0) {
				process[currProcess].meta.push_back(tempMeta);
			}
		}
	//checks for empty file if start is not encountered
	} else if(line.empty()) {
		fin.close();
		throw invalid_argument("The Meta-Data file is empty");
	}
	fin.close();
	fout.close();
}

//returns unsigned int symbolizing memory location
unsigned int allocateMemory( int totMem ) {
	//initializes address so start will be at 0
	static int address = config.systemMemory;

	address += config.memoryBlock;
	if(address > totMem) {
		address = 0;
	}

	return address;
}
