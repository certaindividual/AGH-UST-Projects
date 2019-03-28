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

#define ANSI_COLORED     "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int msgqid;
int *clients;


void handlerINT(int signo) {
	printf("%s...SIGINT received - terminating\n%s",ANSI_COLORED, ANSI_COLOR_RESET);
	exit(EXIT_SUCCESS);
}

void removeQAtExit() {
	msgctl(msgqid, IPC_RMID, NULL);
}

void new_client(struct msgbuf1* message){
	
	int id = 0;    
	while(id < MAX_CLIENTS && clients[id] != -1){
		id++;
	}
    
    if (id < MAX_CLIENTS) {
		//Received in message - client's queue id
        clients[id] = atoi(message->mtext);
        message->mtype = REPLY;
		//Sending back id to a client
        sprintf(message->mtext, "%i", id);
        if (msgsnd(clients[id], message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) {
            perror("Error during sending back initial id");
            clients[id] = -1;
            return;
        }
        printf("%sClient registered, id: %d PID: %d\n%s", ANSI_COLORED, id, message->pid, ANSI_COLOR_RESET);
    }
    else
    {
        message->mtype = ERROR;
        sprintf(message->mtext, "Queue full");
        if (msgsnd(atoi(message->mtext), message, sizeof(struct msgbuf1)-sizeof(long), 0)) perror("Error init msg");
    }
}
void delete_client(struct msgbuf1* message) {
    if (message->clientId < 0 || message->clientId > MAX_CLIENTS) return;
	clients[message->clientId] = -1;
    printf("%sClient removed: id: %d and pid: %d\n%s", ANSI_COLORED, message->clientId, message->pid, ANSI_COLOR_RESET);
	return;    
}

void mirror_fun(int q_id, char* mtext) {

	int stringLength = strlen(mtext);    
    char* stringToSend = (char*)calloc(stringLength, sizeof(char));
	int i=0;
    char *ptr = mtext + strlen(mtext) - 1;

    while (ptr >= mtext) {
		stringToSend[i++] = *ptr--;        
    }
    stringToSend[i] = '\0';    
    struct msgbuf1 message;
    message.mtype = REPLY;
    sprintf(message.mtext, "%s", stringToSend);
    if (msgsnd(q_id, &message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error during sending back mirror text");
}

void errorSend(const char* msgtext, int msgqid) {
    struct msgbuf1 message;
    message.mtype = ERROR;
    sprintf(message.mtext,"%s", msgtext);
    msgsnd(msgqid, &message, sizeof(struct msgbuf1)-sizeof(long), 0);
}


void calc(int clientQueueID, char* message, int message_len) {
    
    int result;

    int tokens_count = 0;
    char **expressions = (char**)calloc(100, sizeof(char*));
    while((expressions[tokens_count] = strtok((tokens_count == 0)? message : NULL, " \n\0")) != NULL){
        tokens_count++;
    }
    if(tokens_count>4) { errorSend("Incorrect calc statement too many tokens", clientQueueID); return; }

    if (strcmp(expressions[0], "CALC") == 0 && tokens_count==2) {

        char** tokens = (char**)calloc(100, sizeof(char*));
        int tokens_nr=0;
        char delims[] = {'+', '-', '*', '/', '\n', '\0'};
        char* theExpression = (char*) calloc(strlen(expressions[1]), sizeof(char));
        strcpy(theExpression, expressions[1]);        
        while((tokens[tokens_nr] = strtok((tokens_nr == 0)? expressions[1] : NULL, delims)) != NULL){
            tokens_nr++;
        }
        if(tokens_nr>3) { errorSend("Incorrect calc statement - too many elems", clientQueueID); return; }
        char* op = (char*)calloc(1, sizeof(char));        
        for(int i=0; i < strlen(theExpression) ;i++){

            if(theExpression[i] == '+') {op[0] = '+'; break;}
            if(theExpression[i] == '-') {op[0] = '-'; break;}
            if(theExpression[i] == '*') {op[0] = '*'; break;}
            if(theExpression[i] == '/') {op[0] = '/'; break;}   
            if(i == strlen(theExpression)-1){ errorSend("Incorrect calc statement - operator error", clientQueueID); return; } 
            
        }     

        int a = atoi(tokens[0]);
        int b = atoi(tokens[1]);        
        switch (op[0]){
            case '+': result = a+b; break;
            case '-': result = a-b; break;
            case '*': result = a*b; break;
            case '/': result = a/b; break;
        }        
        struct msgbuf1 mes;
        mes.mtype = REPLY;
        sprintf(mes.mtext, "%d", result);
        if (msgsnd(clientQueueID, &mes, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error server send");
        return;
    } 
    else if (strcmp(expressions[1], "ADD") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a+b;
        struct msgbuf1 mes;
        mes.mtype = REPLY;
        sprintf(mes.mtext, "%d", result);
        if (msgsnd(clientQueueID, &mes, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error server send");
        return;
    }
    else if (strcmp(expressions[1], "SUB") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a-b;
        struct msgbuf1 mes;
        mes.mtype = REPLY;
        sprintf(mes.mtext, "%d", result);
        if (msgsnd(clientQueueID, &mes, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error server send");
        return;
    }
    else if (strcmp(expressions[1], "MUL") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a*b;
        struct msgbuf1 mes;
        mes.mtype = REPLY;
        sprintf(mes.mtext, "%d", result);
        if (msgsnd(clientQueueID, &mes, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error server send");
        return;
    }
    else if (strcmp(expressions[1], "DIV") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a/b;
        struct msgbuf1 mes;
        mes.mtype = REPLY;
        sprintf(mes.mtext, "%d", result);
        if (msgsnd(clientQueueID, &mes, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Error server send");
        return;
    }
    else {
        errorSend("Incorrect calc statement", clientQueueID); return;
    }
}

void time_fun(int clientQID) {
    struct msgbuf1 message;
    message.mtype = REPLY;

    FILE *date = popen("date", "r");
    if (date == NULL || date < 0) perror("Error using date prog");
    fgets(message.mtext, MSG_MAXSIZE, date);
    pclose(date);
    sprintf(message.mtext, "%.*s", (int)strlen(message.mtext)-1, message.mtext);
    if (msgsnd(clientQID, &message, sizeof(struct msgbuf1)-sizeof(long), 0) < 0) perror("Server send");
}

int main(int argc, char const *argv[]) {
       
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
    clients = (int*)calloc(MAX_CLIENTS,sizeof(int));
    for(int i=0;i<MAX_CLIENTS;i++) {
        clients[i] = -1;
    }
	char *messagesTypes[10] = {"","MIRROR", "CALC", "TIME", "END", "STOP", "INTRODUCE", "ACCEPT", "REPLY", "ERROR"};

	msgqid = msgget(ftok(getenv("HOME"), 0), IPC_CREAT | S_ALLPERMISSIONS);
    
	if(msgqid <0) {

		perror("Error during queue creation:");		
		printf("\n");
		exit(EXIT_FAILURE);
	}
    printf("%sServer queue created id: %d\n%s", ANSI_COLORED, msgqid, ANSI_COLOR_RESET);

	struct msgbuf1 message;

	while(1) {

		if (msgrcv(msgqid, &message, sizeof(struct msgbuf1)-sizeof(long), 0, MSG_NOERROR) < 0) {
			perror("Error during msgreceive:");
			printf("\n");
			exit(EXIT_FAILURE);
		}
		if(1 <= message.mtype && message.mtype <=9) {
			printf("%sReceived %s from client of PID: %d\n%s", ANSI_COLORED, messagesTypes[message.mtype], message.pid, ANSI_COLOR_RESET);
		}
		else {
			printf("%sUnrecognized request received: %s \n %s", ANSI_COLORED, message.mtext, ANSI_COLOR_RESET);
		}

		switch (message.mtype) {
			case INTRODUCE:
				new_client(&message);
				break;
			case MIRROR:
				mirror_fun(clients[message.clientId], message.mtext);
				break;
			case CALC:                
				calc(clients[message.clientId], message.mtext, strlen(message.mtext));
				break;
			case TIME:
				time_fun(clients[message.clientId]);
				break;
			case END:
				printf("Shutting down\n");
    			exit(EXIT_SUCCESS);				 
			case STOP:
                delete_client(&message);
                break;              
		}	
	}
}