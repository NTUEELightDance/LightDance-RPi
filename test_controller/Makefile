CXX = g++
CFLAGS = -Wall -std=c++11 -Wno-psabi -pthread
OPTFLAGS = -g
# OPTFLAGS = -O3

SRC_DIR = src/
INC_DIR = inc/
BIN_DIR = bin/
LIB_DIR = lib/
OBJ_DIR = build/

EXEC = controller
BIN	= $(addprefix $(BIN_DIR), $(EXEC))
INC = -I$(INC_DIR)
SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

# OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
LIB = -L./$(LIB_DIR) -lbcm2835 -lpca9685 -lzmq -lsodium

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ $(LIB) -o $(BIN)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@echo ">> compiling: $<"
	$(CXX) $(CFLAGS) $(INC) $(LIB) $(LIBDEP) -c $< -o $@
clean:
	rm -f $(BIN) $(OBJ)