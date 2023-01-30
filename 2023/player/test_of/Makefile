CC = g++
CFLAGS = -lpthread

SRC = ./
BIN = ./bin
OBJ = ./obj

all : playLoop OFPlayer
	@mkdir -p bin obj
	$(CC) -o $(BIN)/playLoop $(OBJ)/playLoop.o $(OBJ)/OFPlayer.o $(CFLAGS)

playLoop :
	$(CC) -o $(OBJ)/playLoop.o -c playLoop.cpp

OFPlayer :
	$(CC) -o $(OBJ)/OFPlayer.o -c OFPlayer.cpp
