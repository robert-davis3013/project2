CC=gcc
CFLAGS=-g -Wall -std=c11
DEPS = types.h
OBJ = otherCommands.o mkdirSplitPath.o 

.DEFAULT_GOAL := main

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f mkdirSplitPath.o main