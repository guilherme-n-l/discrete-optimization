CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -shared

all: main
	$(MAKE) clean

main: main.o libpq.so
	$(CC) -o main main.o -L. -lpq -O2 -g -lm 

libpq.so: pq.o
	$(CC) $(LDFLAGS) -o libpq.so pq.o

pq.o: pq.c pq.h
	$(CC) $(CFLAGS) -o pq.o -c pq.c

main.o: main.c pq.h
	$(CC) $(CFLAGS) -o main.o -c main.c

clean:
	rm -f pq.o main.o

run:
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./main

debug:
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. gdb --tui ./main
