FLAGS=-L../src -I../src

check: test1
	LD_LIBRARY_PATH=../src ./test1

test1: test1.c
	$(CC) $(CFLAGS) $(FLAGS) -fPIC -g -o $@ $^ -lunclog

clean:
	rm -f test1
