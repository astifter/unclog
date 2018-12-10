LIBNAME=libunclog.so

all: $(LIBNAME)

$(LIBNAME):
	$(MAKE) -C src $@

check: all
	$(MAKE) -C tests $@

clean:
	$(MAKE) -C tests $@
	$(MAKE) -C src $@
