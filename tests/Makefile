FLAGS=-L../src -I../src

check: test1
	LD_LIBRARY_PATH=../src ./test1

test1: test1.c
	$(CC) $(CFLAGS) $(FLAGS) -fPIC -g -o $@ $^ -lunclog

gdb: test1
	LD_LIBRARY_PATH=../src gdb ./test1

clean:
	rm -f test1
