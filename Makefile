CXX = g++
CXXFLAGS = -std=c++14 -fopenmp -Iinc -Wall -Wextra -O3

TARGET = Lab2
SRC = main.cpp $(wildcard inc/*.h) $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

rebuild: all

.PHONY: all rebuild