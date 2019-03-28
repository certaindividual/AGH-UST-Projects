#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include "messageHeader.h"

#define ANSI_COLORED "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int msgqid;
int server;
int clientId = -1;

void handlerINT(int signo) {
	printf(ANSI_COLORED "SIGINT received - terminating\n" ANSI_COLOR_RESET);
	exit(EXIT_SUCCESS);
}
void removeQAtExit() {
	msgctl(msgqid, IPC_RMID, NULL);
}
void connect() {

    struct msgbuf1 message;

    message.mtype = INTRODUCE;
    message.clientId = -1;
    message.pid = getpid();
    sprintf(message.mtext, "%i", msgqid);
    printf(ANSI_COLORED "Client PID: %d sending message: %s to server\n" ANSI_COLOR_RESET, getpid(), message.mtext);
    if (msgsnd(server, &message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) {

		perror("Client sending introduce error");
		printf("\n");
		exit(EXIT_FAILURE);
	}	

    if (msgrcv(msgqid, &message, sizeof(struct msgbuf1)-sizeof(long), 0, 0) < 0) {

		perror("Client receiving reply error");
		printf("\n");
		exit(EXIT_FAILURE);
	}
	
    switch(message.mtype) {

        case REPLY:
            clientId = atoi(message.mtext);
            printf(ANSI_COLORED "Client PID: %d received ID %d from server\n" ANSI_COLOR_RESET, getpid(), clientId);
            if (clientId < 0) {
				perror("Client registering error");
				printf("\n");
				exit(EXIT_FAILURE);
			}			
            break;
        case ERROR:
			perror(message.mtext);
			printf("\n");
			exit(EXIT_FAILURE);            
            break;
        default:
            break;
    }
}
int parse_command(char *line, int len, int* pos) {

    int c = 0;
    int ret;
    char cmnd[6];

    while (line[c] != ' ' && c != len-1 && c < 6) c++;
    sprintf(cmnd, "%.*s", c, line);
    if (line[c] == ' ') while(line[c] == ' ') c++;
    *pos = c;
    
    if (strcmp(cmnd, "MIRROR") == 0)
        ret = MIRROR;
    else if (strcmp(cmnd, "CALC") == 0)
        ret = CALC;
    else if (strcmp(cmnd, "TIME") == 0)
        ret = TIME;
    else if (strcmp(cmnd, "END") == 0)
        ret = END;

    return ret;
}

int main(int argc, char const *argv[]) {

	FILE *file;
    int intr = 1;
    if (argc > 1) {

        if((file = fopen(argv[1], "r"))== NULL) {
			perror("Error during opening the file: ");
			printf("\n");
			exit(EXIT_FAILURE);
		}        
        intr = 0;
    }
    else file = stdin;

	struct sigaction action;
    action.sa_handler = handlerINT;
    sigfillset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL) < -1){
		perror("Error during sigaction\n");
		printf("\n");
		exit(EXIT_FAILURE);
	}
	atexit(removeQAtExit);

	char *buf = NULL;
    char line[100];
    size_t n;
    struct msgbuf1 message;
    int count, pos, message_id;

    if ((server = msgget(ftok(getenv("HOME"), 0), 0)) < 0){
		perror("Error during msgget of server queue\n");
		printf("\n");
		exit(EXIT_FAILURE);
	} 
    if ((msgqid = msgget(IPC_PRIVATE, S_ALLPERMISSIONS)) < 0) {
		perror("Error during msgget of client queue\n");
		printf("\n");
		exit(EXIT_FAILURE);
	}
	connect();

	while ((count = getline(&buf, &n, file)) > 1) {

        sprintf(line, "%.*s", (buf[count-1] == '\n') ? count-1 : count, buf);
        message_id = parse_command(line, count, &pos);
        
        if (message_id != MIRROR && message_id != CALC && message_id != END && message_id != TIME) {
            printf(ANSI_COLORED "Command not recognized: %s\n" ANSI_COLOR_RESET, line);
            continue;
        }
        
        message.mtype = message_id;
        sprintf(message.mtext, "%s", (message_id == MIRROR)? (line+pos) : line);
        message.clientId = clientId;
        message.pid = getpid();
		printf(ANSI_COLORED "Client PID: %d sending message: %s to server\n" ANSI_COLOR_RESET, getpid(), message.mtext);
        if (msgsnd(server, &message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) {
            if (errno == EINVAL && intr)
            {
                printf("Server not found\n");
                continue;
            }
            else {
				perror("Error during msgsnd to server\n");
				printf("\n");
				exit(EXIT_FAILURE);
			}
        }
        if (message.mtype == END) {
			printf(ANSI_COLORED "Client PID: %d ending!\n" ANSI_COLOR_RESET, getpid());
    		exit(EXIT_SUCCESS);	
		}
		if (msgrcv(msgqid, &message, sizeof(struct msgbuf1)-sizeof(long), 0, MSG_NOERROR) < 0) {
			perror("Error during msgreceive:");
			printf("%s\n", message.mtext);			
			exit(EXIT_FAILURE);
		}		
		printf(ANSI_COLORED "Client PID: %d message received from server: %s\n" ANSI_COLOR_RESET, getpid(), message.mtext);     
    }
    message.clientId = clientId;
    message.pid = getpid();
    message.mtype = STOP;
    strcpy(message.mtext, "STOP");    
    if (msgsnd(server, &message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) {
        if (errno == EINVAL && intr) {
            printf("Server not found\n");            
        }
        else {
            perror("Error during msgsnd to server\n");
            printf("\n");
            exit(EXIT_FAILURE);
        }
    }
    printf(ANSI_COLORED "Client PID: %d ending!\n" ANSI_COLOR_RESET, getpid());
    exit(EXIT_SUCCESS);	
}