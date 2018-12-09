LIBNAME=libunclog.so

all: $(LIBNAME)

$(LIBNAME): unclog.c
	$(CC) -Wall -Wextra -pedantic $(CFLAGS) -shared -o $@ $<

check: all
	$(MAKE) -C tests $@

clean:
	$(MAKE) -C tests $@
	rm -f $(LIBNAME)
