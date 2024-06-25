CXX = g++
CXXFLAGS = -std=c++11 -pg -Wall

all: kosaraju_list kosaraju_matrix

kosaraju_list: kosaraju_list.cpp
	$(CXX) $(CXXFLAGS) -o kosaraju_list kosaraju_list.cpp

kosaraju_matrix: kosaraju_matrix.cpp
	$(CXX) $(CXXFLAGS) -o kosaraju_matrix kosaraju_matrix.cpp

profile_list: kosaraju_list
	./kosaraju_list < input7.txt
	gprof --brief kosaraju_list gmon.out > profile_list.txt

profile_matrix: kosaraju_matrix
	./kosaraju_matrix < input7.txt
	gprof --brief kosaraju_matrix gmon.out > profile_matrix.txt

clean:
	rm -f kosaraju_list kosaraju_matrix gmon.out profile_list.txt profile_matrix.txt
