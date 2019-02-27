# asm/Makefile

CC=gcc

FLAGS = -m32 -O2

all: build clean run

build:
	$(CC) $(FLAGS) -c *.c
	$(CC) $(FLAGS) -c *.s
	$(CC) $(FLAGS) *.o -o output -read_only_relocs suppress

clean:
	rm *.o

run:
	./output