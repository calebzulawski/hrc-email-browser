CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic

# change this if you install MITIE elsewhere
MITIE_DIR=MITIE

LIBMITIE=$(MITIE_DIR)/mitielib/libmitie.so

# change this if you place the sqlite3 database elsewhere
DB=../hrcemail.sqlite

LIB_DIR=-L/usr/lib/x86_64-linux-gnu -L$(MITIE_DIR)/mitielib
LIBS=-lsqlite3 -lmitie
INCLUDE=-I$(MITIE_DIR)/mitielib/include

BIN=train

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

.DEFAULT_GOAL=$(BIN)

$(BIN): $(LIBMITIE) $(OBJ)
	$(CXX) -o $(BIN) $(CXXFLAGS) $(OBJ) $(INCLUDE) $(LIB_DIR) $(LIBS)

$(LIBMITIE):
	make -C $(MITIE_DIR)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $^ $(INCLUDE)

run: $(BIN)
	# to run the program, you need to find the .so file
	LD_LIBRARY_PATH=$(MITIE_DIR)/mitielib ./$(BIN)

debug: CXXFLAGS += -g
debug: $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

clobber: clean mitieclean

mitieclean:
	make -C $(MITIE_DIR) clean
