CC = gcc -Wall -std=c11 -ggdb


compile:
	$(CC) -o main main.c -lpthread -lm
	
	
clean:

	rm -f main  *.o *.out	

test:
	./main conf.txt


all: clean compile test



