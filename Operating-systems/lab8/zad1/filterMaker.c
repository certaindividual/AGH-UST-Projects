#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char const *argv[]) {
	/*
	Program przyjmuje w argumentach wywolania:
	>liczbe c
	>nazwe pliku	
	>liczbe d (do dzielenia wartosci - sila filtru)
	*/
	if(argc <4) {
        printf("Niepoprawne argumenty wywolania\n");
        exit(EXIT_FAILURE);
    }
	int c = atoi(argv[1]);
	int n = c*c;
	char* path = (char*)calloc(30,sizeof(char));
	strcpy(path, argv[2]);
	int d = atoi(argv[3]);

	srand(time(NULL));
	FILE *file = fopen(path, "w");
	fprintf(file, "%d\n",c);
	for(int i=0; i<n;i++) {

		double randomN = ((double)rand()/RAND_MAX)/d;
		fprintf(file, "%f\n", randomN);
	}
	fclose(file);
	return 0;

}