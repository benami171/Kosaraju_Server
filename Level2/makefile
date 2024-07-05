CXX = g++
CXXFLAGS = -std=c++11 -pg -Wall

all: Kosaraju_list Kosaraju_deque

Kosaraju_list: Kosaraju_list.cpp Kosaraju.hpp
	$(CXX) $(CXXFLAGS) -o Kosaraju_list Kosaraju_list.cpp

Kosaraju_deque: Kosaraju_deque.cpp Kosaraju.hpp
	$(CXX) $(CXXFLAGS) -o Kosaraju_deque Kosaraju_deque.cpp

profile_list: Kosaraju_list
	./Kosaraju_list < input7.txt
	gprof --brief Kosaraju_list gmon.out > profile_list.txt

profile_deque: Kosaraju_deque
	./Kosaraju_deque < input7.txt
	gprof --brief Kosaraju_deque gmon.out > profile_deque.txt

clean:
	rm -f Kosaraju_list Kosaraju_deque gmon.out profile_list.txt profile_deque.txt
