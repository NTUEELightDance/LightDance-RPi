CC = g++
CFLAGS = -lpthread

SRC = ./
BIN = ./bin
OBJ = ./obj

all : playLoop LEDPlayer OFPlayer
	@mkdir -p bin obj
	$(CC) -o $(BIN)/playLoop $(OBJ)/playLoop.o $(OBJ)/LEDPlayer.o $(OBJ)/OFPlayer.o $(CFLAGS)

playLoop :
	$(CC) -o $(OBJ)/playLoop.o -c playLoop.cpp

LEDPlayer :
	$(CC) -o $(OBJ)/LEDPlayer.o -c LEDPlayer.cpp

OFPlayer :
	$(CC) -o $(OBJ)/OFPlayer.o -c OFPlayer.cpp
