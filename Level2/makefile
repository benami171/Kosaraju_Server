CXX = g++
CXXFLAGS = -std=c++11 -pg -Wall

all: kosaraju_list kosaraju_deque

kosaraju_list: kosaraju_list.cpp kosaraju.hpp
	$(CXX) $(CXXFLAGS) -o kosaraju_list kosaraju_list.cpp

kosaraju_deque: kosaraju_deque.cpp kosaraju.hpp
	$(CXX) $(CXXFLAGS) -o kosaraju_deque kosaraju_deque.cpp

profile_list: kosaraju_list
	./kosaraju_list < input7.txt
	gprof --brief kosaraju_list gmon.out > profile_list.txt

profile_deque: kosaraju_deque
	./kosaraju_deque < input7.txt
	gprof --brief kosaraju_deque gmon.out > profile_deque.txt

clean:
	rm -f kosaraju_list kosaraju_deque gmon.out profile_list.txt profile_deque.txt
