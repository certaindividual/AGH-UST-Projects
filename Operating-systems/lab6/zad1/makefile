CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o server server.c
	$(CC) -o client client.c
	
clean:

	rm -f server client *.o *.out	

test:
	./server &
	./client polecenia1 &
	./client polecenia2 &
	./client polecenia2 &
	./client polecenia1 &
	./client polecenia1 &
	./client polecenia2 &			
	 

all: clean compile test
	

