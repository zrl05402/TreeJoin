CXX=g++-4.8
CXXFLAGS=-std=c++11 -pg -Wall -O3 -g -MMD -MP
TARGET=main

SRC=$(shell ls *.cpp)

all: $(TARGET)

$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) $+ -o $@

-include $(SRC:.cpp=.d)

clean:
	$(RM) *.o *.d *.txt $(TARGET)

.PHONY: all clean

