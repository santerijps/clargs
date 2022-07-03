CC=tcc
FLAGS=-Wall -o bin/clargs.exe

basic: bin
	$(CC) $(FLAGS) examples/basic.c

bin:
	mkdir bin
