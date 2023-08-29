CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm -lc

check_allocator: check_allocator.o allocator.o #../allocator.c
	$(CC) $(CDFLAGS) -o $@ $^ $(LDFLAGS)

check_allocator.o : check_allocator.c ../allocator.h

allocator.o: ../allocator.c ../allocator.h
	$(CC) $(CDFLAGS) -c -o $@ $<

all: check_allocator

.PHONY: run clean

run: check_allocator
	./check_allocator

clean:
	rm -f *.o check_allocator


#con make compili
#con make run compila e esegue
#con make clean elimina i file oggetti derivati da compilazione