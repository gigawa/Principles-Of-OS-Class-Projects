CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : Sim01

Sim01 : Sim01.cpp ConfigData.o
	$(CC) $(LFLAGS) -std=c++11 Sim01.cpp ConfigData.o -o Sim01

ConfigData.o : ConfigData.cpp ConfigData.h
	$(CC) $(CFLAGS) -c -std=c++11 ConfigData.cpp

clean:
	\rm *.o Sim01
