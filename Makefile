PREFIX:=/usr/local

all: build/bin/test1

# actual build targets
build:
	mkdir build

build/Makefile: build
	cd build && cmake ..

build/bin/test1: build/Makefile
	cd build && make VERBOSE=1

install: build/bin/test1
	cd build && /usr/local/bin/cmake -DCMAKE_INSTALL_PREFIX=$(PREFIX) -DCOMPONENT=Main -P cmake_install.cmake
	rm build/install_manifest_*

# clean and rebuild
clean:
	rm -rf build staging documentation

rebuild: clean build/bin/test1

# checkers
check: install_testing
	cd staging/tests && LD_LIBRARY_PATH=../lib ./test1

gdb: install_testing
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ ./test1

valgrind: install_testing
	cd staging/tests && LD_LIBRARY_PATH=../lib $@ --leak-check=full --track-origins=yes ./test1

install_testing:
	$(MAKE) PREFIX=$(PWD)/staging install
	cd build && /usr/local/bin/cmake -DCMAKE_INSTALL_PREFIX=$(PWD)/staging -DCOMPONENT=Test -P cmake_install.cmake
	rm build/install_manifest_*

# documentaton and formating
doc: format documentation/html/index.html

format: build/Makefile
	cd build && make VERBOSE=1 format

documentation/html/index.html: unclog.doxygen src/*.c src/*.h README.md
	doxygen $< && open $@
