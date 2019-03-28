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
#include <string.h>
#include <limits.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"



int N;
int *children;
int master;

void handler_INT(int sig, siginfo_t *info, void *ucontext) {

	kill(master, SIGINT);
    waitpid(master, NULL, 0);
	for (int i = 0; i < N; i++){
		if (children[i] != 0) {
            kill(children[i], SIGKILL);
            waitpid(children[i], NULL, 0);
        }
	}
    exit(EXIT_SUCCESS);	
}

int main(int argc, char *argv[]) {    

    if (argc < 4){
        printf("Zbyt malo argumentow wywolania.\n");
		printf("Potrzebne: [1]PATH [2]SLAVE_NR [3]N\n");
        exit(EXIT_FAILURE);
    }
	
	char *PATH = (char*)calloc(4096,sizeof(char));	
	strcpy(PATH, argv[1]);
    int SLAVE_NR = (int) strtol(argv[2], NULL, 10);;
    N = (int) strtol(argv[3], NULL, 10);

	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = handler_INT;
	sigemptyset(&action.sa_mask);	
	sigaction(SIGINT, &action, NULL);

	children = (int*)calloc(SLAVE_NR, sizeof(int));
	
	master = fork();

	if(master == 0){
		execlp("./master", "master", PATH, NULL);
		perror("Cos poszlo nie tak:");
		printf("\n");
	}
	struct stat statbuf;
	while(stat(PATH,&statbuf) <0) {		
		sleep(1);
	}

    for (int i = 0; i < SLAVE_NR; i++) {		
        children[i] = fork();
        if (children[i] == 0){
            execlp("./slave", "./slave", argv[1], argv[3], 0);
			perror("Cos poszlo nie tak:");
			printf("\n");
			exit(EXIT_FAILURE);
        }		
    }  
    
    waitpid(master, NULL, 0);
	printf("%s Master zakonczyl dzialanie - KONIEC %s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);     
	
	raise(SIGINT);

    return 0;
}