all:
	make clean
	g++ -I ~/boost_1_81_0 list.cpp player.cpp -o list.o ~/boost_1_81_0/stage/lib/libboost_serialization.a -Wall
	g++ -I ~/boost_1_81_0 load.cpp player.cpp -o load.o ~/boost_1_81_0/stage/lib/libboost_serialization.a -Wall
clean:
	rm -f list.o
	rm -f load.o