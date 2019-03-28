CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o barber barber.c -lrt -pthread
	$(CC) -o client client.c -lrt -pthread
	
	
	
clean:

	rm -f barber client *.o *.out	

test:
	./barber 3 &
	./client 5 3 &
	 

all: clean compile test
