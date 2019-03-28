#include <stdlib.h>
#include <stdio.h>



int main() {
	
	int a =0;
	int b = 5;
	for(int i=0; i<10000000; i++) {
		int *tab = (int*)malloc(100000*sizeof(int));
		int tmp = a;
		a = b;
		b = tmp;
		free(tab);		
	}
	return 0;
}
