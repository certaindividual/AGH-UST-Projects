CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c
	$(CC) -o lamaczCzasu lamaczCzasu.c
	$(CC) -o lamaczPamieci lamaczPamieci.c
	$(CC) -o programik programik.c
clean:

	rm -f main lamaczCzasu lamaczPamieci programik *.o *.out

testGOOD:
	./main poleceniaGOOD 30 20

testERR:
	./main poleceniaERR 10 10

allgood: clean compile testGOOD

allerr: clean compile testERR
	

