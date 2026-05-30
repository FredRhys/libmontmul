CC = gcc
LD = $(CC)
CFLAGS = -std=c23\
		 -Wall\
		 -Werror\
		 -Wextra\
		 -pedantic\
		 -O3\
		 -march=native\
		 -fopenmp

main: montmul.c
	$(LD) $(CFLAGS) montmul.c -o montmul

debug: montmul.c
	$(LD) $(CFLAGS) -fsanitize=address -fsanitize=undefined -g montmul.c -o montmul

default: main