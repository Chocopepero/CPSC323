CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
SOURCES = main.cpp lexer.cpp syntax_analyzer.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = syntax_analyzer

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -static -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean