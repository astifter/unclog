AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog -lcunit

check_PROGRAMS = unclog_test_1 unclog_tests_1
TESTS = unclog_tests_1

unclog_test_1_LDADD = ../unclog/libunclog.la
unclog_test_1_SOURCES = test1.c

unclog_tests_1_LDADD = ../unclog/libunclog.la
unclog_tests_1_SOURCES = unclog_tests_1.c

gdb: unclog_tests_1
	LD_LIBRARY_PATH=../unclog/.libs gdb ./.libs/unclog_tests_1

valgrind: unclog_tests_1
	LD_LIBRARY_PATH=../unclog/.libs valgrind --leak-check=full --track-origins=yes ./.libs/unclog_tests_1
