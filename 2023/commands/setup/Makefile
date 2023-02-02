all:
	make clean
	g++ list.cpp player.cpp -o list.o -lboost_serialization -Wall
	g++ load.cpp player.cpp -o load.o -lboost_serialization -Wall
clean:
	rm -f list.o
	rm -f load.o