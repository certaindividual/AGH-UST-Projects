CC = gcc -Wall -std=c11 -ggdb


compile:
	$(CC) -o main main.c -lpthread -lm
	$(CC) -o filterMaker filterMaker.c
	
clean:

	rm -f main filterMaker *.o *.out	

test:
	./filterMaker 5 filter5 100
	./filterMaker 15 filter15 200
	./filterMaker 30 filter30 500
	./filterMaker 45 filter45 1000
	./filterMaker 60 filter60 1300

	
	for filterType in 5 15 30 45 60; do \
		for threadsNr in 1 2 4 8; do \
			echo "---------------------------------------------------------------"; \
			echo "Threads nr: $$threadsNr, filter: filter$$filterType"; \
			./main $$threadsNr image filter$$filterType outputImage$$threadsNr$$filterType \
			rm dane; \
		done; \
	done

littletest:
	./filterMaker 30 filter30 500
	./main 8 portrait filter30 outLittleTest

all: clean compile test

all1: clean compile littletest


