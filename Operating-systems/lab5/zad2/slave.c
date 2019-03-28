#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main(int argc, char *argv[]) {

	int PID = getpid();
	printf("%s Nowy Slave PID: %d %s\n", ANSI_COLOR_GREEN, PID, ANSI_COLOR_RESET);
		
	if(argc <3) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
	char *path = argv[1];
	
    int fifo;
	if((fifo = open(path, O_WRONLY)) <0) {
		perror("Blad przy otwieraniu kolejki: ");
		printf("\n");
		return 1;
	} 

	char *pipe_buf = (char*)calloc(PIPE_BUF,sizeof(char));

	int N = (int) strtol(argv[2], NULL, 10);		

    for (int i = 0; i < N; i++) {

        FILE *fp_date = popen("date", "r");
		char* date = (char*)calloc(PIPE_BUF,sizeof(char));
        fgets(date, PIPE_BUF, fp_date);
		       
        sprintf(pipe_buf,"Slave PID: %d - %s", (int)getpid(), date);		
		write(fifo, pipe_buf, strlen(pipe_buf));		
        srand(((unsigned)getpid())+time(NULL));
		unsigned sleep_interval = (unsigned) ((rand() % 3)+2);
        sleep(sleep_interval);
    }
	close(fifo);
	return 0;
}