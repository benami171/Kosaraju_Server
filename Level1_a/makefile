CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCS = kosaraju_server.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = kosaraju

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)