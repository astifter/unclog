check: test1
	LD_LIBRARY_PATH=../src:../inih ./test1

test1: test1.c
	$(CC) $(CFLAGS) -o $@ $^ -lunclog -linih -lpthread

gdb: test1
	LD_LIBRARY_PATH=../src:../inih gdb ./test1

clean:
	rm -f test1
