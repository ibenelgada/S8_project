CC=g++
CFLAGS= -std=c++11 -Wall -O -g
LDFLAGS=
EXEC=raptor


all: mkdir $(EXEC)

raptor: main.o Stop.o Route.o Network.o Position.o Bag.o Label.o
				$(CC) -o bin/raptor obj/Bag.o obj/Label.o obj/Route.o obj/Network.o obj/Position.o obj/Stop.o obj/main.o $(LDFLAGS)

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
