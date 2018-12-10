export LIBNAME:=libunclog.so
export CFLAGS:=-Wall -Wextra -pedantic -fPIC -g $(CFLAGS)

all: $(LIBNAME)

$(LIBNAME):
	$(MAKE) -C src $@

check: all
	$(MAKE) -C tests $@

gdb: all
	$(MAKE) -C tests $@

clean:
	$(MAKE) -C tests $@
	$(MAKE) -C src $@
