CC = gcc -m64
CFLAGS = -Iinclude -Wall -Werror -Wpedantic
OBJS = src/main.o src/eval.o src/env.o src/parser.o src/tokenize.o src/util.o

all: main.exe

main.exe: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main.exe

src/main.o: src/main.c include/util.h include/env.h include/tokenize.h include/parser.h include/eval.h
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/eval.o: src/eval.c include/eval.h include/util.h include/tokenize.h include/parser.h include/env.h
	$(CC) $(CFLAGS) -c src/eval.c -o src/eval.o

src/parser.o: src/parser.c include/parser.h include/util.h include/tokenize.h
	$(CC) $(CFLAGS) -c src/parser.c -o src/parser.o

src/tokenize.o: src/tokenize.c include/tokenize.h include/util.h
	$(CC) $(CFLAGS) -c src/tokenize.c -o src/tokenize.o

src/util.o: src/util.c include/util.h
	$(CC) $(CFLAGS) -c src/util.c -o src/util.o

src/env.o: src/env.c include/env.h 
	$(CC) $(CFLAGS) -c src/env.c -o src/env.o

clean:
	rm -f src/*.o main.exe	
