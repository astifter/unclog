all: build build/Makefile build/bin/test1

build:
	mkdir build

build/Makefile:
	cd build && cmake ..

build/bin/test1:
	cd build && make VERBOSE=1

clean:
	rm -rf build

format check valgrind gdb: all
	cd build && make VERBOSE=1 $@
