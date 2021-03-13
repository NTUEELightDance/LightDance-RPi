CXX = g++
CFLAGS = -Wall -std=c++11 -Wno-psabi
OPTFLAGS = -g
# OPTFLAGS = -O3

SRC_DIR = src/
INC_DIR = inc/
BIN_DIR = bin/
LIB_DIR = lib/

EXEC = LEDTest
BIN	= $(addprefix $(BIN_DIR), $(EXEC))
INC = -I$(INC_DIR)
SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:%.cpp=%.o)
LIB = -L./$(LIB_DIR) -lbcm2835

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ $(LIB) -o $(BIN)

%.o : %.cpp
	@echo ">> compiling: $<"
	$(CXX) $(CFLAGS) $(INC) $(LIB) $(LIBDEP) -c $< -o $@
clean:
	rm -f $(BIN) $(OBJ)
