CXX = g++
CXXFLAGS = -std=c++17 -Wall -static -static-libgcc -static-libstdc++

SRCS = main.cpp lexer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = lexer

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean