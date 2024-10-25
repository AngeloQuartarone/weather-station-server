SHELL=/bin/bash
CC=gcc
CFLAGS=-Wall -Wextra -pedantic 
VFLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes -s 

default: data-retrieval

all: data-retrieval

data-retrieval: ./data-retrieval.o ./lib/circularQueue.o ./lib/linkedList.o ./lib/zambretti.o
	$(CC) $(CFLAGS) $^ -o $@ -lpaho-mqtt3c -lm `mysql_config --cflags --libs`

data-retrieval.o: data-retrieval.c ./lib/circularQueue.h ./lib/linkedList.h ./lib/zambretti.h
circularQueue.o: ./lib/circularQueue.c ./lib/circularQueue.h
linkedList.o: ./lib/linkedList.c ./lib/linkedList.h
zambretti.o: ./lib/zambretti.c ./lib/zambretti.h

clean:
	rm -f *.o data-retrieval ./lib/*.o