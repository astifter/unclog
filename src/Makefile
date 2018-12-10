$(LIBNAME): unclog.c unclog_init.c
	$(CC) $(CFLAGS) -shared -o $@ $^ -linih

clean:
	rm -f $(LIBNAME)
