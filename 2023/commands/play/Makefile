CC = g++
CFLAGS = -std=c++14 -Wall
LIBS = -lpthread -lboost_serialization

all:
	make clean read send
clean:
	rm -f read.o
	rm -f send.o
read:
	$(CC) readtest.cpp -o read.o $(CFLAGS)
send:
	$(CC) main.cpp -o send.o $(CFLAGS)