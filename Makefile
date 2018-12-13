all: build build/Makefile build/bin/test1

build:
	mkdir build

build/Makefile: CMakeLists.txt src/CMakeLists.txt
	cd build && cmake ..

build/bin/test1:
	cd build && make

check: all
	cd build && make test

clean:
	rm -rf build
