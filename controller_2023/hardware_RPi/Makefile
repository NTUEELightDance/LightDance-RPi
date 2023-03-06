CXX = g++
CFLAGS = -std=c++17 -Wall
LIBS = -lpthread -lboost_serialization

SRC_DIR = src/
INC_DIR = inc/
BIN_DIR = bin/
LIB_DIR = lib/
OBJ_DIR = build/

INC = -I$(INC_DIR)
SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

all: directories $(OBJ)

directories: $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@echo ">> compiling: $<"
	$(CXX) $(CFLAGS) $(INC) $(LIB) $(LIBDEP) -c $< -o $@

clean:
	rm -f $(OBJ)