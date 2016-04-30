CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
INCLUDE=-L/usr/lib/x86_64-linux-gnu
LIBS=-lsqlite3
BIN=train

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)
MITIE_DIR=MITIE
MITIE_OBJ=mitielib/libmitie.a

all: $(BIN)

$(BIN): $(MITIE_DIR)/$(MITIE_OBJ) $(OBJ)
	$(CXX) -o $(BIN) $^ $(INCLUDE) $(LIBS)

$(MITIE_DIR)/$(MITIE_OBJ):
	make -C $(MITIE_DIR)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^

clean:
	rm -f *.o
	rm -f $(BIN)

clobber: clean mitieclean

mitieclean:
	make -C $(MITIE_DIR) clean
