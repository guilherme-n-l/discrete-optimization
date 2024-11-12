CC = gcc
CFLAGS = -O2 `pkg-config --cflags glib-2.0` -g
LDFLAGS = `pkg-config --libs glib-2.0`
default: run

run: main.c
	$(CC) main.c $(CFLAGS) $(LDFLAGS) -o main

.PHONY: clean
clean:
	rm -f main
