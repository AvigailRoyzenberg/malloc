CFLAGS=-std=gnu99 -g

test: my_mem.o test.o

my_mem.o: my_mem.c my_malloc.h
test.o: test.c my_malloc.h

clean:
	rm *.o test
