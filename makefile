# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

all: libProactor libReactor libKosaraju
	make -C Level_01 all
	make -C Level_02 all
	make -C Level_02b all
	make -C Level_03 all
	make -C Level_04 all
	make -C Level_05_06 all
	make -C Level_07_08_09 all
	make -C Level_10 all

libReactor: Reactor.o
	ar rcs libReactor.a Reactor.o

libProactor: Proactor.o
	ar rcs libProactor.a Proactor.o

libKosaraju: Kosaraju.o
	ar rcs libKosaraju.a Kosaraju.o

Kosaraju.o: Kosaraju.cpp
	$(CC) $(CFLAGS) -c Kosaraju.cpp

Reactor.o: Reactor.cpp 
	$(CC) $(CFLAGS) -c Reactor.cpp

Proactor.o: Proactor.cpp
	$(CC) $(CFLAGS) -c Proactor.cpp

clean: 
	rm *.o *.a
	make -C Level_01 clean
	make -C Level_02 clean
	make -C Level_02b clean
	make -C Level_03 clean
	make -C Level_04 clean
	make -C Level_05_06 clean
	make -C Level_07_08_09 clean
	make -C Level_10 clean

.PHONY: clean Level_01 Level_02 Level_02b Level_03 Level_04 Level_05_06 Level_07_08_09 Level_10