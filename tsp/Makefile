CC = gcc
CFLAGS = -O2 -g -lm
LDFLAGS = 
default: run

run: main.c
	$(CC) main.c $(CFLAGS) $(LDFLAGS) -o main

.PHONY: clean
clean:
	rm -f main
