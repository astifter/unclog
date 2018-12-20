AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog

check_PROGRAMS = unclog_test_1
TESTS = unclog_test_1

unclog_test_1_LDADD = ../unclog/libunclog.la
unclog_test_1_SOURCES = test1.c

gdb: unclog_test_1
	LD_LIBRARY_PATH=../unclog/.libs gdb ./.libs/unclog_test_1

valgrind: unclog_test_1
	LD_LIBRARY_PATH=../unclog/.libs valgrind --leak-check=full --track-origins=yes ./.libs/unclog_test_1
