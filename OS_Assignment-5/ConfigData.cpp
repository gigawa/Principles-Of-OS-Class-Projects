#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "ConfigData.h"

using namespace std;

void ConfigData::read(char * filename) {
	string line = filename;
	//finds the last period marking the file extension
	size_t extension = line.rfind(".");
	line = line.substr(extension+1);

	//checks for the correct file extension
	if(line != "conf") {
		throw invalid_argument("Incorrect file extension for Config file");
	}

	//opens file if it can be opened
	fstream fin(filename);
	if(!fin) {
		fin.close();
		throw invalid_argument("Config file cannot be opened");
	}
	fin >> ws;

	//resets line then reads in first line of file
	line = "";
	getline(fin, line);

	//checks for start of configuration file
	if(line == "Start Simulator Configuration File") {
		//checks for end of file
		while(line != "End") {
			fin >> line;
			//determines which cycle time is read in
			if(line == "Version/Phase:") {
				fin >> version;
			} else if(line == "File") {
				getline(fin, line, ':');
				fin >> filePath;
			} else if(line == "Processor") {
				getline(fin, line, ':');
				fin >> processor;
			} else if(line == "Monitor") {
				getline(fin, line, ':');
				fin >> monitor;
			} else if(line == "Hard") {
				fin >> ws;
				getline(fin, line, ':');
				if(line == "drive quantity"){
					fin >>hardDriveQuantity;
				}else {
					fin >> hardDrive;
				}
			} else if(line == "Printer") {
				fin >> ws;
				getline(fin, line, ':');
				if(line == "quantity"){
					fin >>printerQuantity;
				}else {
					fin >> printer;
				}
			} else if(line == "Keyboard") {
				getline(fin, line, ':');
				fin >> keyboard;
			} else if(line == "Memory") {
				fin >> line;
				if(line == "block") {
					int multiplier = 1;
					getline(fin, line, '(');
					getline(fin, line, ')');
					if(line == "Mbytes") {
						multiplier = 1024;
					}else if(line == "Gbytes") {
						multiplier = 1048576;
					}
					getline(fin, line, ':');
					fin >> memoryBlock;
					memoryBlock *= multiplier;
				}else {
					getline(fin, line, ':');
					fin >> memory;
				}
			} else if(line == "Mouse") {
				getline(fin, line, ':');
				fin >> mouse;
			} else if(line == "Speaker") {
				getline(fin, line, ':');
				fin >> speaker;
			} else if(line == "Log:") {
				fin >> ws;
				getline(fin, log);
			} else if(line == "Log") {
				getline(fin, line, ':');
				fin >> logFilePath;
			} else if(line == "System") {
				int multiplier = 1;
				getline(fin, line, '(');
				getline(fin, line, ')');
				if(line == "Mbytes") {
					multiplier = 1024;
				}else if(line == "Gbytes") {
					multiplier = 1048576;
				}
				getline(fin, line, ':');
				fin >> systemMemory;
				systemMemory *= multiplier;
			} else if(line == "CPU") {
				getline(fin, line, ':');
				fin >> cpuSchedule;
			}
		}
	//if start is not encountered it checks if the file is empty
	} else if(line.empty()) {
		fin.close();
		throw invalid_argument("The Config file is empty");
	}
	fin.close();
}

void ConfigData::print() {
	//logs to the monitor
	if(log == "Log to Both" || log == "Log to Monitor") {
		cout << "Configuration File Data" << endl;
		cout << "Processor = " << processor << " ms/cycle" << endl;
		cout << "Monitor = " << monitor << " ms/cycle" << endl;
		cout << "Hard Drive = " << hardDrive << " ms/cycle" << endl;
		cout << "Printer = " << printer << " ms/cycle" << endl;
		cout << "Keyboard = " << keyboard << " ms/cycle" << endl;
		cout << "Memory = " << memory << " ms/cycle" << endl;
		cout << "Mouse = " << mouse << " ms/cycle" << endl;
		cout << "Speaker = " << speaker << " ms/cycle" << endl;
		cout << "Logged to: monitor";
		if(log == "Log to Both") {
			cout << " and " << logFilePath << endl << endl;
		} else {
			cout << endl << endl;
		}
	}

	//logs to the file specified
	if(log == "Log to Both" || log == "Log to File") {
		ofstream fout;
		fout.open(logFilePath);
		fout << "Configuration File Data" << endl;
		fout << "Processor = " << processor << " ms/cycle" << endl;
		fout << "Monitor = " << monitor << " ms/cycle" << endl;
		fout << "Hard Drive = " << hardDrive << " ms/cycle" << endl;
		fout << "Printer = " << printer << " ms/cycle" << endl;
		fout << "Keyboard = " << keyboard << " ms/cycle" << endl;
		fout << "Memory = " << memory << " ms/cycle" << endl;
		fout << "Mouse = " << mouse << " ms/cycle" << endl;
		fout << "Speaker = " << speaker << " ms/cycle" << endl << endl;
		if(log == "Log to File") {
			cout << "Logged to: " << logFilePath << endl;
		}
	}
}
