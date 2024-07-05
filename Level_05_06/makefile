# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

# Libraries
LIBS = -lpthread -lReactor -lKosaraju

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = main

# Build target
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) -L.. $(LIBS)

# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
