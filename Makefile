all: build build/Makefile build/bin/test1

build:
	mkdir build

build/Makefile: CMakeLists.txt src/CMakeLists.txt
	cd build && cmake ..

build/bin/test1:
	cd build && make VERBOSE=1

check: all
	cd build && make check

format:
	cd build && make VERBOSE=1 format

clean:
	rm -rf build
