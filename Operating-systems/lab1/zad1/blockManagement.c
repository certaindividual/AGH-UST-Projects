// Mateusz Monasterski

#include "blockManagement.h"

struct arrayOfBlocks{
	char **array;
	int size;	
	int isDynamic;	
};


char staticArray[STATIC_ARRAY_SIZE][STATIC_ARRAY_BLOCK_SIZE];

struct arrayOfBlocks* createArray(int size, int isDynamic){
	if(size <= 0)
		return NULL;
	struct arrayOfBlocks* newArray = (struct arrayOfBlocks*) calloc(1, sizeof(struct arrayOfBlocks));
	newArray->size = size;	
	newArray->isDynamic = isDynamic;
	
	if(isDynamic) {
		char** charArray = (char**) calloc(size, sizeof(char*));
		newArray->array = charArray;		
	}
	else {
		newArray->array = (char**) staticArray;
	}
	return newArray;
}

void deleteArray(struct arrayOfBlocks* toDel) {
	if(toDel==NULL) return;
	
	if(toDel->isDynamic) {
		for(int i=0;i<toDel->size;i++) {
			if(toDel->array[i]!=NULL){
				free(toDel->array[i]);
			}			
		}
	}
	else {
		for(int i=0;i<STATIC_ARRAY_SIZE;i++){
			toDel->array[i]= "";
		}
	}
}

void addBlock(struct arrayOfBlocks* arrayToAddBlockTo, char* block, int index) {
	if(arrayToAddBlockTo == NULL) return;
	if(arrayToAddBlockTo->size <= index || index <0){
		printf("Index %d does not exist \n", index);
		return;
	}
	if(arrayToAddBlockTo->isDynamic){
		if(arrayToAddBlockTo->array[index] != NULL)
			free(arrayToAddBlockTo->array[index]);
	
		arrayToAddBlockTo->array[index] = calloc(strlen(block), sizeof(char));
		strcpy(arrayToAddBlockTo->array[index], block);
	}
	else {
		if(strlen(block) > STATIC_ARRAY_BLOCK_SIZE){
			printf("The block is too big for this array");
			return;
		}
		strcpy(arrayToAddBlockTo->array[index], block);
	}
	
		
}

void deleteBlock(struct arrayOfBlocks* arrayToDelBlockFrom, int index) {
	if(arrayToDelBlockFrom == NULL) return;
	if(arrayToDelBlockFrom->size <= index || index <0){
		printf("Index %d does not exist \n", index);
		return;
	}
	if(arrayToDelBlockFrom->isDynamic){
		if(arrayToDelBlockFrom->array[index] != NULL){
			free(arrayToDelBlockFrom->array[index]);
			arrayToDelBlockFrom->array[index] = NULL;
		}	
	}
	else{
		arrayToDelBlockFrom->array[index] = "";		
	}	
}

int sumOfASCII(char* block) {
	int counter = 0;
    int lengthOfBlock = strlen(block);
    for (int i = 0; i < lengthOfBlock; i++)
        counter += (int) block[i];
    return counter;
}

char* findElemOfTheClosestASCIISum(struct arrayOfBlocks* arrayP, int val) {
	char* result = NULL;
	int minDifference = INT_MAX;
	for (int i = 0; i < arrayP->size; i++) {        
        if (arrayP->array[i] != NULL) {
            int difference = abs(sumOfASCII(arrayP->array[i]) - val);
            if (minDifference > difference) {
                minDifference = difference;
                result = arrayP->array[i];
            }
        }
    }
    return result;
}


