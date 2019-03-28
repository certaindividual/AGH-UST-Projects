CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c
	$(CC) -o master master.c
	$(CC) -o slave slave.c
	
clean:

	rm -f master slave FIFO main  *.o *.out

test:
	./main FIFO 5 3

all: clean compile test
	

