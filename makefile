CC = gcc -m64
CFLAGS = -Iinclude -Wall -Werror -Wpedantic

all: prog
prog: src/lang.o
	$(CC) $(CFLAGS) src/lang.o -o lang.exe
lang.o: src/lang.c
	$(CC) $(CFLAGS) -c src/lang.c -o src/lang.o
clean:
	rm -f src/lang.o
