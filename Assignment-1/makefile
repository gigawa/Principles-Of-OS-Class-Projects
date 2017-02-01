all : Sim01

Sim01 : Sim01.cpp ConfigData.o
	g++ -std=c++11 Sim01.cpp ConfigData.o -o Sim01

ConfigData.o : ConfigData.cpp ConfigData.h
	g++ -c -std=c++11 ConfigData.cpp

clean:
	\rm *.o Sim01
