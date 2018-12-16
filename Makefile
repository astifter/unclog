default: valgrind
all: install format valgrind

build:
	mkdir build

build/Makefile: build
	cd build && cmake ..

build/bin/test1: build/Makefile
	cd build && make VERBOSE=1

install: build/bin/test1
	cd build && /usr/local/bin/cmake -DCMAKE_INSTALL_PREFIX=$(PWD)/staging -P cmake_install.cmake

clean:
	rm -rf build staging

rebuild: clean install

format: build/Makefile
	cd build && make VERBOSE=1 format

check: install
	cd staging/tests && LD_LIBRARY_PATH=../lib ./test1

gdb: install
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ ./test1

valgrind: install
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ --leak-check=full --track-origins=yes ./test1
