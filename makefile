CC = gcc
LD = $(CC)
CFLAGS = -std=c23\
		 -g\
		 -Wall\
		 -pedantic\
		 -fsanitize=undefined\
		 -fsanitize=address

test: test.o montmul.o
	$(LD) $(CFLAGS) test.o montmul.o -o test

test.o: test.c montmul.h
	$(LD) $(CFLAGS) -c test.c -o test.o

montmul.o: montmul.c montmul.h
	$(LD) $(CFLAGS) -c montmul.c -o montmul.o

.PHONY: clean
clean:
	rm -rf test test.o montmul.o

default: test