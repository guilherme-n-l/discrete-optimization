CC = gcc
CFLAGS = -std=c99 -O2 -lm

default: run

run: main.c
	$(CC) $(CFLAGS)	-o main main.c

.PHONY: clean
clean:
	rm -f main
