AM_CFLAGS = --std=c99 -Wall -Wextra -pedantic -I../inih

lib_LTLIBRARIES = libunclog.la
libunclog_la_LIBADD = ../inih/libinih.a
libunclog_la_SOURCES = unclog.c unclog_config.c unclog_global.c unclog_levels.c unclog_sink.c unclog_source.c unclog_int.h unclog/unclog.h
libunclog_la_LDFLAGS = -version-info 1:0:0

nobase_include_HEADERS = unclog/unclog.h
