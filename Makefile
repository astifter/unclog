export LIBINIH_ROOT=$(PWD)/inih
export LIBNAME:=libunclog.so
export CFLAGS:=--std=c99 -Wall -Wextra -pedantic -fPIC -g -I$(LIBINIH_ROOT) -I$(PWD)/src -L$(LIBINIH_ROOT) -L$(PWD)/src $(CFLAGS) 

all: check

$(LIBNAME):
	$(MAKE) -C inih
	$(MAKE) -C src $@

check: $(LIBNAME)
	$(MAKE) -C tests $@

gdb: all
	$(MAKE) -C tests $@

clean:
	$(MAKE) -C tests $@
	$(MAKE) -C src $@
	$(MAKE) -C inih clean
