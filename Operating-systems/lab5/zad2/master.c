#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>


int main(int argc, char *argv[]) {
	
	if(argc <2) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
	char *path = argv[1];


	if(mkfifo(path, S_IRWXU) < 0) {
		perror("Blad przy tworzeniu kolejki: ");
		printf("\n");
		return 1;
	}
	char *pipe_buf = (char*)calloc(PIPE_BUF,sizeof(char));

    FILE *fifo;
	if((fifo = fopen(path, "r")) == NULL) {
		perror("Blad przy otwieraniu kolejki: ");
		printf("\n");
		return 1;
	} 
	int i=1;
    while (fgets(pipe_buf, PIPE_BUF, fifo) != NULL) {
        printf("[%d] %s", i, pipe_buf);
		i++;
    }
    fclose(fifo);
	return 0;
}