CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Default target
all: kosaraju_interactive

# Compile the interactive Kosaraju program
kosaraju_interactive: kosaraju_interactive.cpp
	$(CXX) $(CXXFLAGS) -o kosaraju_interactive kosaraju_interactive.cpp

# Run the interactive Kosaraju program with input file
run: kosaraju_interactive
	./kosaraju_interactive < input_commands.txt

# Clean up generated files
clean:
	rm -f kosaraju_interactive

.PHONY: all run profile clean
