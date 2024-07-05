CXX = g++
CXXFLAGS = -std=c++11 -pg -Wall

all: Kosaraju_list Kosaraju_matrix

Kosaraju_list: Kosaraju_list.cpp
	$(CXX) $(CXXFLAGS) -o Kosaraju_list Kosaraju_list.cpp

Kosaraju_matrix: Kosaraju_matrix.cpp
	$(CXX) $(CXXFLAGS) -o Kosaraju_matrix Kosaraju_matrix.cpp

profile_list: Kosaraju_list
	./Kosaraju_list < input7.txt
	gprof --brief Kosaraju_list gmon.out > profile_list.txt

profile_matrix: Kosaraju_matrix
	./Kosaraju_matrix < input7.txt
	gprof --brief Kosaraju_matrix gmon.out > profile_matrix.txt

clean:
	rm -f Kosaraju_list Kosaraju_matrix gmon.out profile_list.txt profile_matrix.txt
