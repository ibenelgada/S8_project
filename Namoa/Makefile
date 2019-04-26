CC=g++
CFLAGS= -std=c++11 -Wall -O -g
LDFLAGS=
EXEC=namoa


all: mkdir $(EXEC)

namoa: main.o Position.o Label.o Cost.o Arc.o Node.o Graph.o Json.o functions.o
				$(CC) -o bin/namoa obj/Json.o obj/Label.o obj/Arc.o obj/Node.o obj/Graph.o obj/Position.o obj/Cost.o obj/functions.o obj/main.o $(LDFLAGS)

%.o: sources/%.cpp
				$(CC) -o obj/$@ -c $< $(CFLAGS)

mkdir:
	[ -d bin ] || mkdir -p bin; [ -d obj ] || mkdir -p obj

clean:
				rm -f obj/*.o core

clear: clean
			  rm -f bin/*
