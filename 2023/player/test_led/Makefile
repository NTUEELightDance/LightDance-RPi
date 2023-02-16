CC = g++
CFLAGS = -std=c++14
LIBS = -lpthread -lboost_serialization

SRC = .
BIN = ./bin
OBJ = ./obj

all : playLoop LEDPlayer LEDController
	@mkdir -p $(BIN)
	$(CC) -o $(BIN)/playLoop \
		$(OBJ)/playLoop.o \
		$(OBJ)/LEDPlayer.o \
		$(OBJ)/hardware/LEDController.o \
		$(CFLAGS) $(LIBS)

playLoop :
	@mkdir -p $(OBJ)
	$(CC) -o $(OBJ)/playLoop.o \
		-c $(SRC)/playLoop.cpp \
		$(CFLAGS)

LEDPlayer : 
	@mkdir -p $(OBJ)
	$(CC) -o $(OBJ)/LEDPlayer.o \
		-c $(SRC)/LEDPlayer.cpp \
		$(CFLAGS)

LEDController : 
	@mkdir -p $(OBJ)/hardware
	$(CC) -o $(OBJ)/hardware/LEDController.o \
		-c $(SRC)/hardware/LEDController.cpp \
		$(CFLAGS)

