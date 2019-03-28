#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include "blockManagement.h"

struct arrayOfBlocks{
	char **array;
	int size;	
	int isDynamic;	
};


char* randomizeString(int len) {
	if(len<1) return NULL;
	char* range = "ABCDEFGHIJKLMNOPRSTUWVXYZabcdefghijklmnoprstuwvxyz0123456789!@#$%^&*";
	int rangeNr = strlen(range);
	char *randomStr = (char*) calloc(len, sizeof(char));
	randomStr[len-1] = '\0';
	for(int i=0; i<len-1;i++) {
		randomStr[i] = range[rand() % rangeNr];
	}
	return randomStr;
}

void fillArray(struct arrayOfBlocks* array, int blockSize) {
	for(int i =0; i<((*array).size);i++) 
		addBlock(array, randomizeString(blockSize),i);	
}

void addXBlocks(struct arrayOfBlocks* array, int x, int blockSize, int from) {
	for(int i = from; i<(from+x);i++) {
		addBlock(array, randomizeString(blockSize),i);	
	}
}

void removeXBlocks(struct arrayOfBlocks* array, int x, int from){
	for(int i =from; i<from+x;i++) 
		deleteBlock(array, i);
}

void removeXThenAddXBlocks(struct arrayOfBlocks* array, int x, int from, int blockSize){
	removeXBlocks(array, x, from);
	addXBlocks(array, x, from, blockSize);
}

void repeatRemove_AddXTimes(struct arrayOfBlocks* array, int x, int blockSize){
	for(int i=0;i<x;i++) {
		deleteBlock(array,i);
		addBlock(array, randomizeString(blockSize), i);
	}
}

double calTime(clock_t start, clock_t end) {
	return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void operation(struct arrayOfBlocks* array, char* toDo, int how, int blockSize, int from){
	
	if(strcmp(toDo, "search_element")==0)
		findElemOfTheClosestASCIISum(array, how);
	
	else if(strcmp(toDo, "remove")==0)
		removeXBlocks(array, how, from);
		
	else if(strcmp(toDo, "add")==0)
		addXBlocks(array, how, blockSize, from);
	else if(strcmp(toDo, "remove_and_add")==0)
		repeatRemove_AddXTimes(array, how, blockSize);
	else {
		printf("Wrong operation");
		return;
	}
		
}

int main(int nrOfArgs, char **arg) {
	srand((unsigned int) time(NULL));
	if(nrOfArgs <4) {
		printf("Wrong argument list\n minimum arguments needed:\n");
		printf("arraySize(int), blockSize (int), allocation type(static=0|dynamic=1)");
		return 1;
	}
	
	int arraySize = atoi(arg[1]);
	int blockSize = atoi(arg[2]);
	if(atoi(arg[3])!=0 && atoi(arg[3]) != 1) {
		printf("Wrong type of allocation number!");
		return 1;
	}
	int isDynamic = atoi(arg[3]);
	
	struct tms** tms_arr = calloc(20, sizeof(struct tms*));
	
	clock_t realTime[20];
	
	for(int i =0; i<20;i++) {
		tms_arr[i] = (struct tms *) calloc(1, sizeof(struct tms*));
	}
	
	printf("    Real    User    System\n");
	
	
	realTime[0] = times(tms_arr[0]);
	
	
	struct arrayOfBlocks* array = createArray(arraySize, isDynamic);
	
	realTime[1] = times(tms_arr[1]);
	
	printf("creation\n");
	printf("    "); printf("%lf", calTime(realTime[0], realTime[1]));
	printf("    "); printf("%lf", calTime(tms_arr[0]->tms_utime, tms_arr[1]->tms_utime));
	printf("    "); printf("%lf", calTime(tms_arr[0]->tms_stime, tms_arr[1]->tms_stime));
	printf("\n");
	
	realTime[2] = times(tms_arr[2]);
	
	fillArray(array, blockSize);
	
	realTime[3] = times(tms_arr[3]);
	
	printf("filling in\n");
	printf("    "); printf("%lf", calTime(realTime[2], realTime[3]));
	printf("    "); printf("%lf", calTime(tms_arr[2]->tms_utime, tms_arr[3]->tms_utime));
	printf("    "); printf("%lf", calTime(tms_arr[2]->tms_stime, tms_arr[3]->tms_stime));
	printf("\n");
	
	int rem = 4;
	int nrArg = 4;
	char* order;
	int how, from;
		
	while(nrArg < nrOfArgs) {
		order = arg[nrArg];
		if(strcmp(order, "add")==0 || strcmp(order, "remove")==0){
			int incr = 3;
			how = atoi(arg[nrArg+1]);
			from = atoi(arg[nrArg+2]);
						
			realTime[rem] = times(tms_arr[rem]);
	
			operation(array, order, how, blockSize, from);
	
			realTime[rem+1] = times(tms_arr[rem+1]);
	
			printf(order); printf("\n");
			printf("    "); printf("%lf", calTime(realTime[rem], realTime[rem+1]));
			printf("    "); printf("%lf", calTime(tms_arr[rem]->tms_utime, tms_arr[rem+1]->tms_utime));
			printf("    "); printf("%lf", calTime(tms_arr[rem]->tms_stime, tms_arr[rem+1]->tms_stime));
			printf("\n");
			rem +=2;
			nrArg += incr;
		}
		else {
			int incr = 2;
			how = atoi(arg[nrArg+1]);
							
			realTime[rem] = times(tms_arr[rem]);
	
			operation(array, order, how, blockSize, from);
	
			realTime[rem+1] = times(tms_arr[rem+1]);
	
			printf(order); printf("\n");
			printf("    "); printf("%lf", calTime(realTime[rem], realTime[rem+1]));
			printf("    "); printf("%lf", calTime(tms_arr[rem]->tms_utime, tms_arr[rem+1]->tms_utime));
			printf("    "); printf("%lf", calTime(tms_arr[rem]->tms_stime, tms_arr[rem+1]->tms_stime));
			printf("\n");
			rem +=2;
			nrArg += incr;
		}
	}
	
	return 0;
	
}
