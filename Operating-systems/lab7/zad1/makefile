CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o barber barber.c
	$(CC) -o client client.c
	
	
clean:

	rm -f barber client *.o *.out	

test:
	./barber 10 &
	./client 25 4 &
	 

all: clean compile test


