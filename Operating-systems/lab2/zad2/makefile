CC = gcc -Wall -std=c11 -ggdb

compile:
	$(CC) -o main main.c

clean:

	rm -f main *.o *.out

testNFTW:
	./main "/etc" ">" "10.10.2010 10:10:10" nftw

testSTAT:
	./main "/etc" ">" "10.10.2010 10:10:10" nftw