CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c
	
clean:

	rm -f main  *.o *.out

test1:
	./main 20 1
test2:
	./main 20 2
test3:
	./main 20 3

all1: clean compile test1
all2: clean compile test2
all3: clean compile test3
	

