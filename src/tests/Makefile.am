AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog

check_PROGRAMS = unclog_test_1 unclog_test_2
TESTS = unclog_test_2

unclog_test_1_LDADD = ../unclog/libunclog.la
unclog_test_1_SOURCES = test1.c

unclog_test_2_LDADD = ../unclog/libunclog.la ../unclog/libunclog_test.a
unclog_test_2_SOURCES = test_open_close.c

gdb: unclog_test_2
	LD_LIBRARY_PATH=../unclog/.libs gdb ./.libs/unclog_test_2

valgrind: unclog_test_1
	LD_LIBRARY_PATH=../unclog/.libs valgrind --leak-check=full --track-origins=yes ./.libs/unclog_test_1
