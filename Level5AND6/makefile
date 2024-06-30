# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

# Libraries
LIBS = -lpthread

# Source files
SRCS = main.cpp kosaraju.cpp

# Header files
HEADERS = Reactor.hpp kosaraju.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = main

# Build target
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

# Pattern rule for object files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
