CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c
	$(CC) -o wrong_program wrong_program.c
clean:

	rm -f main wrong_program *.o *.out

test:
	./main polecenia

all: clean compile test
	

