AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../unclog -lcunit

check_PROGRAMS = unclog_tests_t unclog_tests_1
TESTS = unclog_tests_1

unclog_tests_t_LDADD = ../unclog/libunclog.la
unclog_tests_t_SOURCES = unclog_tests_t.c

unclog_tests_1_LDADD = ../unclog/libunclog.la
unclog_tests_1_SOURCES = unclog_tests_1.c

gdb: unclog_tests_1
	LD_LIBRARY_PATH=../unclog/.libs gdb ./.libs/unclog_tests_1

gprof: unclog_tests_t
	rm -rf gmon.out
	gcc -std=c99 -pg -O3 -I../unclog -o $< ./unclog_tests_t.c \
	    ../unclog/.libs/unclog*.o ../inih/ini.o -lpthread
	./$< 2>/dev/null
	gprof ./$< gmon.out | less

valgrind: unclog_tests_1
	LD_LIBRARY_PATH=../unclog/.libs valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./.libs/unclog_tests_1
