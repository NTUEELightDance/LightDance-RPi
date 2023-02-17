CC = g++
CFLAGS = -std=c++14 -Wall
LIBS = -lpthread -lboost_serialization

all:
	make clean list load
clean:
	rm -f list.out
	rm -f load.out
list:
	$(CC) list.cpp player.cpp LEDPlayer.cpp OFPlayer.cpp -o list.out $(CFLAGS) $(LIBS)
load:
	$(CC) load.cpp player.cpp LEDPlayer.cpp OFPlayer.cpp -o load.out $(CFLAGS) $(LIBS)
