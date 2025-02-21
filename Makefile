CC=gcc
CFLAGS=-Wextra -Wall -ggdb -I./include
LDFLAGS=-L./lib
LIBS=-lraylib -lm

fabrik: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)
