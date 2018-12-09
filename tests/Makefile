FLAGS=-L.. -I..

check: test1

test1: test1.c
	$(CC) $(CFLAGS) $(FLAGS) -o $@ $< -lunclog
	LD_LIBRARY_PATH=.. ./test1

clean:
	rm -f test1
