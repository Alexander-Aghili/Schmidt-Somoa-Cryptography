SHELL := /bin/sh
CC=clang
CFLAGS=-Wall -Wextra -Werror -Wpedantic -Wshadow $(shell pkg-config --cflags gmp)
LFLAGS=$(shell pkg-config --libs gmp)

SRCFILES=numtheory.c randstate.c ss.c argparser.c 
OBJFILES=numtheory.o randstate.o ss.o argparser.o 
HEADERS=argparser.h numtheory.h randstate.h ss.h

all: encrypt decrypt keygen

decrypt: decrypt.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

keygen: keygen.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

argparser.o: argparser.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

randstate.o: randstate.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

numtheory.o: numtheory.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

ss.o: ss.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o decrypt encrypt keygen

format:
	clang-format -i -style=file *.[ch]
