CC = g++
CFLAGS = -std=c++14
LIBS = -lpthread -lboost_serialization

SRC = ./
BIN = ./bin
OBJ = ./obj

all : playLoop save LEDPlayer
	@mkdir -p bin obj
	$(CC) -o $(BIN)/playLoop $(OBJ)/playLoop.o $(OBJ)/LEDPlayer.o $(CFLAGS) $(LIBS)
	$(CC) -o $(BIN)/save $(OBJ)/save.o $(OBJ)/LEDPlayer.o $(CFLAGS) $(LIBS)

playLoop :
	$(CC) -o $(OBJ)/playLoop.o -c playLoop.cpp $(CFLAGS)

save :
	$(CC) -o $(OBJ)/save.o -c save.cpp $(CFLAGS)

LEDPlayer :
	$(CC) -o $(OBJ)/LEDPlayer.o -c LEDPlayer.cpp $(CFLAGS)
