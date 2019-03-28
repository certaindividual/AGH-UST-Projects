CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o server server.c -lrt
	$(CC) -o client client.c -lrt
	
	
clean:

	rm -f server client *.o *.out	

test:	
	./server &
	./client polecenia1 &	
	./client polecenia2 &
	./client polecenia2 &
	./client polecenia1 &
	./client polecenia2 &
	./client polecenia1 &
	./client polecenia2 &
	sleep 5
	./client polecenieKoncowe
	
		
	

all: clean compile test
