AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog

EXTRA_DIST = unclog.ini

noinst_PROGRAMS = example1

example1_LDADD = ../unclog/libunclog.la
example1_SOURCES = example1.c

examples: example1
