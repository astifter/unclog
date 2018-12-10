LIBNAME=libunclog.so

$(LIBNAME): unclog.c unclog_init.c
	$(CC) -Wall -Wextra -pedantic $(CFLAGS) -shared -fPIC -g -o $@ $^ -linih

clean:
	rm -f $(LIBNAME)
