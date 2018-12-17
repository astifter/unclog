ACLOCAL_AMFLAGS = -Im4

SUBDIRS = src
EXTRA_DIST = autogen.sh

# valgrind testing
valgrind:
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ --leak-check=full --track-origins=yes ./test1

# documentaton and formating
doc: format documentation/html/index.html

format: build/Makefile
	SOURCES=$(find src -name '*.c') \
	HEADERS=$(find src -name '*.h') \
	clang-format-3.8 -i -style=file $${SOURCES} $${HEADERS}

documentation/html/index.html: unclog.doxygen src/unclog*.c src/unclog*.h README.md
	doxygen $< && open $@
