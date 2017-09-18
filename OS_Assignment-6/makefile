CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : sim05

sim05 : sim05.cpp ConfigData.o
	$(CC) $(LFLAGS) -std=c++11 sim05.cpp ConfigData.o -o sim05 -lpthread

ConfigData.o : ConfigData.cpp ConfigData.h
	$(CC) $(CFLAGS) -c -std=c++11 ConfigData.cpp

clean:
	\rm *.o sim05
