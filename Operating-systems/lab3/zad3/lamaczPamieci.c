#include <stdlib.h>
#include <stdio.h>

#define TOO_BIG_NUMBER 1000000000000

int main() {
	int a[TOO_BIG_NUMBER];
	int *b = (int*) malloc(TOO_BIG_NUMBER*sizeof(int));
	return 0;
}
