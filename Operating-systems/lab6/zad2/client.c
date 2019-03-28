#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/msg.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include "messageHeader.h"

#define ANSI_COLORED "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

mqd_t msgqid;
mqd_t server;
int clientId = -1;
char queue_name[8];

void handlerINT(int signo) {
	printf(ANSI_COLORED "SIGINT received - terminating\n" ANSI_COLOR_RESET);
	exit(EXIT_SUCCESS);
}

void connect(char* queue_name) {

    char message[MSG_MAX];

    message[0] = INTRODUCE;
    message[1] = (char)-1;    
    sprintf(message+2, "%s", queue_name);
    printf(ANSI_COLORED "Client PID: %d sending message: %s to server\n" ANSI_COLOR_RESET, getpid(), message+2);
    if (mq_send(server, message, MSG_MAX, 0) < 0) {

		perror("Client sending introduce error");
		printf("\n");
		exit(EXIT_FAILURE);
	}	

    if (mq_receive(msgqid, message, MSG_MAX, NULL) < 0) {

		perror("Client receiving reply error");
		printf("\n");
		exit(EXIT_FAILURE);
	}
	
    switch(message[0]) {

        case REPLY:
            clientId = atoi(message+2);
            printf(ANSI_COLORED "Client PID: %d received ID %d from server\n" ANSI_COLOR_RESET, getpid(), clientId);
            if (clientId < 0) {
				perror("Client registering error");
				printf("\n");
				exit(EXIT_FAILURE);
			}			
            break;
        case ERROR:
			perror(message+2);
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
void removeQAtExit() {
	if (clientId != -1) {
        char msg[MSG_MAX];
        msg[0] = STOP;
        msg[1] = (char)clientId;
        sprintf(msg+2, "%i", clientId);
        if (mq_send(server, msg, MSG_MAX, 1) < 0) perror("Exit send");
    }

    if (mq_close(server) < 0) perror("Exit close server");
    if (mq_close(msgqid) < 0) perror("Exit close client");
    if (mq_unlink(queue_name) < 0) perror("Exit unlink client");
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

    char message[MSG_MAX];
    int count, pos;
    char message_id;
    sprintf(queue_name, "/%iq", getpid());
    

    if ((server = mq_open("/server", O_WRONLY, NULL)) < 0){
		perror("Error during mq_open of server queue\n");
		printf("\n");
		exit(EXIT_FAILURE);
	} 
    if ((msgqid = mq_open(queue_name, O_CREAT | O_EXCL | O_RDONLY, S_IRUSR | S_IWUSR, NULL)) < 0) {
		perror("Error during mq_open of client queue\n");
		printf("\n");
		exit(EXIT_FAILURE);
	}
    
	connect(queue_name);

	while ((count = getline(&buf, &n, file)) > 1) {

        sprintf(line, "%.*s", (buf[count-1] == '\n') ? count-1 : count, buf);
        message_id = parse_command(line, count, &pos);
        
        if (message_id != MIRROR && message_id != CALC && message_id != END && message_id != TIME) {
            printf(ANSI_COLORED "Command not recognized: %s\n" ANSI_COLOR_RESET, line);
            continue;
        }
        
        message[0] = message_id;
        sprintf(message+2, "%s", (message_id == MIRROR)? (line+pos) : line);
        message[1] = (char)clientId;        
		printf(ANSI_COLORED "Client PID: %d sending message: %s to server\n" ANSI_COLOR_RESET, getpid(), message+2);


        if (mq_send(server, message, MSG_MAX, 0) < 0) {
            if (errno == EBADF && intr)
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
        if (message[0] == END) {
			printf(ANSI_COLORED "Client PID: %d ending!\n" ANSI_COLOR_RESET, getpid());
    		exit(EXIT_SUCCESS);	
		}   

		if (mq_receive(msgqid, message, MSG_MAX, NULL) < 0) {
			perror("Error during msgreceive:");
			printf("%s\n", message+2);			
			exit(EXIT_FAILURE);
		}	        
		printf(ANSI_COLORED "Client PID: %d message received from server: %s \n" ANSI_COLOR_RESET, getpid(), message+2);     
    }

    printf(ANSI_COLORED "Client PID: %d ending!\n" ANSI_COLOR_RESET, getpid());
    exit(EXIT_SUCCESS);	
}