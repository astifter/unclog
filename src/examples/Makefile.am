AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog -lcunit

EXTRA_DIST = unclog.ini

noinst_PROGRAMS = example1

example1_LDADD = ../unclog/libunclog.la
example1_SOURCES = example1.c

examples: example1
	./example1

gdb: example1
	LD_LIBRARY_PATH=../unclog/.libs gdb ./.libs/example1

valgrind: example1
	LD_LIBRARY_PATH=../unclog/.libs valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./.libs/example1
