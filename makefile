CC = gcc -m64
CFLAGS = -Iinclude -Iexternal/unity -Iexternal/uthash  -Werror -Wpedantic
OBJS = build/eval.o build/env.o build/parser.o build/tokenize.o build/util.o

all: bin/main.exe

run: bin/main.exe
	./bin/main.exe


bin/main.exe: $(OBJS) build/main.o 
	$(CC) $(CFLAGS) $(OBJS) build/main.o  -o bin/main.exe


tests: bin/run_tests.exe
	./bin/run_tests.exe

bin/run_tests.exe: $(OBJS) build/tests/test_eval.o build/tests/unity.o
	$(CC) $(CFLAGS) $(OBJS) build/tests/test_eval.o build/tests/unity.o  -o bin/run_tests.exe

build/tests/unity.o: external/unity/unity.c
	$(CC) $(CFLAGS) -c external/unity/unity.c -o build/tests/unity.o

build/tests/test_eval.o: tests/test_eval.c include/util.h include/env.h include/tokenize.h include/parser.h include/eval.h
	$(CC) $(CFLAGS) -c tests/test_eval.c -o build/tests/test_eval.o



build/main.o: src/main.c include/util.h include/env.h include/tokenize.h include/parser.h include/eval.h
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/eval.o: src/eval.c include/eval.h include/util.h include/tokenize.h include/parser.h include/env.h
	$(CC) $(CFLAGS) -c src/eval.c -o build/eval.o

build/parser.o: src/parser.c include/parser.h include/util.h include/tokenize.h
	$(CC) $(CFLAGS) -c src/parser.c -o build/parser.o

build/tokenize.o: src/tokenize.c include/tokenize.h include/util.h
	$(CC) $(CFLAGS) -c src/tokenize.c -o build/tokenize.o

build/util.o: src/util.c include/util.h
	$(CC) $(CFLAGS) -c src/util.c -o build/util.o

build/env.o: src/env.c include/env.h 
	$(CC) $(CFLAGS) -c src/env.c -o build/env.o



clean:
	rm -f build/*.o build/tests/*.o bin/main.exe bin/run_tests.exe	

