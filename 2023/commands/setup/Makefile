CC = g++
CFLAGS = -std=c++14 -Wall
LIBS = -lpthread -lboost_serialization

all:
	make clean list load
clean:
	rm -f list.o
	rm -f load.o
list:
	$(CC) list.cpp player.cpp LEDPlayer.cpp -o list.o $(CFLAGS) $(LIBS)
load:
	$(CC) load.cpp player.cpp LEDPlayer.cpp -o load.o $(CFLAGS) $(LIBS)
