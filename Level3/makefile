CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Default target
all: Kosaraju_interactive

# Compile the interactive Kosaraju program
Kosaraju_interactive: Kosaraju_interactive.cpp
	$(CXX) $(CXXFLAGS) -o Kosaraju_interactive Kosaraju_interactive.cpp

# Run the interactive Kosaraju program with input file
run: Kosaraju_interactive
	./Kosaraju_interactive < input_commands.txt

# Clean up generated files
clean:
	rm -f Kosaraju_interactive

.PHONY: all run profile clean
