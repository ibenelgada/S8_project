CC=g++
CFLAGS= -std=c++11 -Wall -O
LDFLAGS=
EXEC=raptor


all: mkdir $(EXEC)

raptor: main.o
				$(CC) -o bin/raptor obj/main.o $(LDFLAGS)

%.o: sources/%.cpp
				$(CC) -o obj/$@ -c $< $(CFLAGS)

mkdir:
	[ -d bin ] || mkdir -p bin; [ -d obj ] || mkdir -p obj

clean:
				rm -f obj/*.o core

clear: clean
			  rm -f bin/*
