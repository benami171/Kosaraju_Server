# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

all: libProactor libReactor libKosaraju
	make -C Level1 all
	make -C Level2 all
	make -C Level2_b all
	make -C Level3 all
	make -C Level4 all
	make -C Level5_6 all
	make -C Level7_8_9 all
	make -C Level10 all

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
	make -C Level1 clean
	make -C Level2 clean
	make -C Level2_b clean
	make -C Level3 clean
	make -C Level4 clean
	make -C Level5_6 clean
	make -C Level7_8_9 clean
	make -C Level10 clean

.PHONY: clean Level1 Level2 Level2_b Level3 Level4 Level5_6 Level7_8_9 Level10