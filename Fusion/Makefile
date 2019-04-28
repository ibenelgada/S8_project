CC=g++
CFLAGS= -std=c++11 -Wall -O -g
LDFLAGS=
EXEC=fusion


all: mkdir $(EXEC)

fusion: main.o Cost.o Arc.o Node.o Graph.o Json.o functions.o Stop.o Route.o Network.o Position.o RouteInfo.o Bag.o RouteInfo.o Label.o Bags.o
				$(CC) -o bin/fusion obj/Bag.o obj/Bags.o obj/Cost.o obj/Label.o obj/RouteInfo.o obj/Route.o obj/Network.o obj/Position.o obj/Stop.o  obj/Json.o obj/Arc.o obj/Node.o obj/Graph.o obj/functions.o obj/main.o $(LDFLAGS)

test: test.o Bag.o Label.o
				$(CC) -o bin/test obj/test.o obj/Bag.o obj/Label.o $(LDFLAGS)

%.o: sources/%.cpp
				$(CC) -o obj/$@ -c $< $(CFLAGS)

mkdir:
	[ -d bin ] || mkdir -p bin; [ -d obj ] || mkdir -p obj

clean:
				rm -f obj/*.o core

clear: clean
			  rm -f bin/*
