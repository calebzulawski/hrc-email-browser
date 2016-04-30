CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
INCLUDE=-L/usr/lib/x86_64-linux-gnu
LIBS=-lsqlite3
BIN=train

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^ $(INCLUDE) $(LIBS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^

clean:
	rm -f *.o
	rm -f $(BIN)
