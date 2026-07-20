CC = gcc
LD = $(CC)
CFLAGS = -std=c23\
		 -g\
		 -Wall\
		 -pedantic\
		 -fsanitize=undefined\
		 -fsanitize=address

.PHONY: test
test: test.o montmul.o
	$(LD) $(CFLAGS) -o montmul test.o montmul.o 

test.o: test.c montmul.h

montmul.o: montmul.c montmul.h

default: test