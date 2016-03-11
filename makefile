CC      ?= gcc
CFLAGS  ?= -Wall -Werror -std=c99
PROG = test

all: test_file.c lib_example.o
	$(CC) $(CFLAGS) -o $(PROG) test_file.c lib_example.o -lm

lib_example.o: lib_example.c lib_example.h
	$(CC) $(CFLAGS) -c lib_example.c
