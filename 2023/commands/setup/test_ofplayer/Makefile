CC = g++
CFLAGS = -std=c++14
LIBS = -lpthread -lboost_serialization

SRC = ./
BIN = ./bin
OBJ = ./obj

all : playLoop save OFPlayer
	@mkdir -p bin obj
	$(CC) -o $(BIN)/playLoop $(OBJ)/playLoop.o $(OBJ)/OFPlayer.o $(CFLAGS)
	$(CC) -o $(BIN)/save $(OBJ)/save.o $(OBJ)/OFPlayer.o $(CFLAGS) $(LIBS)

playLoop :
	$(CC) -o $(OBJ)/playLoop.o -c playLoop.cpp

save :
	$(CC) -o $(OBJ)/save.o -c save.cpp $(CFLAGS)

OFPlayer :
	$(CC) -o $(OBJ)/OFPlayer.o -c OFPlayer.cpp
