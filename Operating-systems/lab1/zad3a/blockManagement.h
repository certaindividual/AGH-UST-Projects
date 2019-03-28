#ifndef _BLOCK_MANAGEMENT_H_
#define _BLOCK_MANAGEMENT_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define STATIC_ARRAY_SIZE 500000
#define STATIC_ARRAY_BLOCK_SIZE 1000

struct arrayOfBlocks* createArray(int size, int isDynamic);
void deleteArray(struct arrayOfBlocks* toDel);
void addBlock(struct arrayOfBlocks* arrayToAddBlockTo, char* block, int index);
void deleteBlock(struct arrayOfBlocks* arrayToDelBlockFrom, int index);
int sumOfASCII(char* block);
char* findElemOfTheClosestASCIISum(struct arrayOfBlocks* arrayP, int val);


#endif
