default: check
all: install

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

rebuild: clean default

format: build/Makefile
	cd build && make VERBOSE=1 format

check: install
	cd staging/tests && LD_LIBRARY_PATH=../lib ./test1

valgrind gdb: install
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ ./test1
