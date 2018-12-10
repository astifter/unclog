LIBNAME=libunclog.so

$(LIBNAME): unclog.c
	$(CC) -Wall -Wextra -pedantic $(CFLAGS) -shared -o $@ $<

clean:
	rm -f $(LIBNAME)
