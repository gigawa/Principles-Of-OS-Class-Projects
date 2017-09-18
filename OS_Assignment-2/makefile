CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : Sim02

Sim02 : Sim02.cpp ConfigData.o
	$(CC) $(LFLAGS) -std=c++11 Sim02.cpp ConfigData.o -o Sim02 -lpthread

ConfigData.o : ConfigData.cpp ConfigData.h
	$(CC) $(CFLAGS) -c -std=c++11 ConfigData.cpp

clean:
	\rm *.o Sim02
