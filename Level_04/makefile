# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall -g

# Libraries
LIBS = -lpthread

# Source files
SRCS = multipleUsers.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = multipleUsers

# Build target
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
