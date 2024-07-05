# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

# Libraries
LIBS = -lpthread -lProactor -lReactor -lKosaraju

# Source files
SRCS = main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = main

# Valgrinf flags
VALGRIND_FLAGS = --tool=memcheck -v --leak-check=full --show-leak-kinds=all --error-exitcode=99

# Valgrind check for both executables
valgrind: $(EXEC) 
	valgrind $(VALGRIND_FLAGS) ./$(EXEC) 2>&1

# Build target
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) -L.. $(LIBS)

# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
