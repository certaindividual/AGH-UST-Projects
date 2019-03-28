CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c

clean:

	rm -f main *.o *.out

test:
	./main "/etc" ">" "10.10.2010 10:10:10" nftw



all: clean compile test
