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
#include <mqueue.h>
#include "messageHeader.h"

#define ANSI_COLORED     "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

mqd_t msgqid;
mqd_t *clients;


void handlerINT(int signo) {
	printf("%s...SIGINT received - terminating\n%s",ANSI_COLORED, ANSI_COLOR_RESET);
    mq_close(msgqid);
    mq_unlink("/server");
	exit(EXIT_SUCCESS);
}

void removeQAtExit() {
	
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != -1) if (mq_close(clients[i]) < 0) perror("Client remove");
    mq_close(msgqid);
    mq_unlink("/server");
}

void new_client(char* message){
	
	int id = 0;    
	
    char name[MSG_MAX-2];
    mqd_t clientQID;
    sprintf(name, "%s", message+2);
    if ((clientQID = mq_open(name, O_WRONLY, NULL)) < 0) {
        perror("Error during client queue open: ");
        printf("\n");
        exit(EXIT_FAILURE);
    }       
    while(id < MAX_CLIENTS && clients[id] != -1){
		id++;
	}
    
    if (id < MAX_CLIENTS) {

        clients[id] = clientQID;
        message[0] = REPLY;
		//Sending back id to a client
        sprintf(message+2, "%i", id);
        if (mq_send(clients[id], message, MSG_MAX, 1) < 0) {
            perror("Error during sending back initial id");
            clients[id] = (mqd_t)-1;
            return;
        }
        printf("%sClient registered, id: %d\n%s", ANSI_COLORED, id, ANSI_COLOR_RESET);
    }
    else
    {
        message[0] = ERROR;
        sprintf(message+2, "Queue full");
        if (mq_send(clients[id], message, MSG_MAX, 1)) perror("Error init msg");
    }
}
void delete_client(char* message) {
    if (message[0] < 0 || message[1]> MAX_CLIENTS) return;    
    if (mq_close(clients[(int)message[1]]) < 0) perror("Remove client error:");
	clients[(int)message[1]] = -1;
    printf("%sClient removed: id: %d\n%s", ANSI_COLORED, (int)message[1], ANSI_COLOR_RESET);
	return;    
}

void mirror_fun(mqd_t q_id, char* mtext) {

	int stringLength = strlen(mtext);    
    char* stringToSend = (char*)calloc(stringLength, sizeof(char));
	int i=0;
    char *ptr = mtext + strlen(mtext) - 1;

    while (ptr >= mtext) {
		stringToSend[i++] = *ptr--;        
    }
    stringToSend[i] = '\0'; 

    char message[MSG_MAX];
    message[0] = REPLY;
    sprintf(message+2, "%s", stringToSend);    
    if (mq_send(q_id, message, MSG_MAX, 1) < 0) perror("Error during sending back mirror text");    
}

void errorSend(const char* msgtext, mqd_t msgqid) {


    char message[MSG_MAX];
    message[0] = ERROR;
    sprintf(message+2, "%s", msgtext);
    mq_send(msgqid, message, MSG_MAX, 1);    
}


void calc(mqd_t clientQueueID, char* message, int message_len) {
    
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

        char mes[MSG_MAX];
        mes[0] = REPLY;
        sprintf(mes+2, "%d", result);        
        
        if (mq_send(clientQueueID, mes, MSG_MAX, 1) < 0) perror("Error server send");          
        return;
    } 
    else if (strcmp(expressions[1], "ADD") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a+b;
        char mes[MSG_MAX];
        mes[0] = REPLY;
        sprintf(mes+2, "%d", result);
        if (mq_send(clientQueueID, mes, MSG_MAX, 1) < 0) perror("Error server send");          
        return;
    }
    else if (strcmp(expressions[1], "SUB") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a-b;
        char mes[MSG_MAX];
        mes[0] = REPLY;
        sprintf(mes+2, "%d", result);
        if (mq_send(clientQueueID, mes, MSG_MAX, 1) < 0) perror("Error server send");          
        return;
    }
    else if (strcmp(expressions[1], "MUL") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a*b;
        char mes[MSG_MAX];
        mes[0] = REPLY;
        sprintf(mes+2, "%d", result);
        if (mq_send(clientQueueID, mes, MSG_MAX, 1) < 0) perror("Error server send");          
        return;
    }
    else if (strcmp(expressions[1], "DIV") == 0 && tokens_count == 4) 
    {
        int a = atoi(expressions[2]);
        int b = atoi(expressions[3]);
        result = a/b;
        char mes[MSG_MAX];
        mes[0] = REPLY;
        sprintf(mes+2, "%d", result);
        if (mq_send(clientQueueID, mes, MSG_MAX, 1) < 0) perror("Error server send");          
        return;
    }
    else {
        errorSend("Incorrect calc statement", clientQueueID); return;
    }
}

void time_fun(mqd_t clientQID) {

    char mes[MSG_MAX];
    mes[0] = REPLY;

    FILE *date = popen("date", "r");
    if (date == NULL || date < 0) perror("Error using date prog");
    fgets(mes+2, MSG_MAX-2, date);
    pclose(date);

    sprintf(mes+2, "%.*s", (int)strlen(mes+2)-1, mes+2);
    if (mq_send(clientQID, mes, MSG_MAX, 1) < 0) perror("Server send");

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
    clients = (mqd_t*)calloc(MAX_CLIENTS,sizeof(mqd_t));
    for(int i=0;i<MAX_CLIENTS;i++) {
        clients[i] = (mqd_t)-1;
    }
	char *messagesTypes[10] = {"","MIRROR", "CALC", "TIME", "END", "STOP", "INTRODUCE", "ACCEPT", "REPLY", "ERROR"};

	msgqid = mq_open("/server", O_CREAT | O_EXCL | O_RDONLY, S_IRUSR | S_IWUSR, NULL);
    
	if(msgqid <0) {

		perror("Error during queue creation:");		
		printf("\n");
		exit(EXIT_FAILURE);
	}
    printf("%sServer queue created id: %d\n%s", ANSI_COLORED, msgqid, ANSI_COLOR_RESET);

    char message[MSG_MAX];  

	while(1) {

		if (mq_receive(msgqid, message, MSG_MAX, NULL) < 0) {
			perror("Error during msgreceive:");
			printf("\n");
			exit(EXIT_FAILURE);
		}
		if(1 <= message[0] && message[0] <=9) {
			printf("%sReceived %s from client: %s\n%s", ANSI_COLORED, messagesTypes[(int)message[0]],message+2, ANSI_COLOR_RESET);
		}
		else {
			printf("%sUnrecognized request received: %s \n %s", ANSI_COLORED, message+2, ANSI_COLOR_RESET);
		}

		switch (message[0]) {
			case INTRODUCE:
				new_client(message);
				break;
			case MIRROR:
				mirror_fun(clients[(int)message[1]], message+2);
				break;
			case CALC:                
				calc(clients[(int)message[1]], message+2, strlen(message+2));
				break;
			case TIME:
				time_fun(clients[(int)message[1]]);
				break;
			case END:
				printf("Shutting down\n");
    			exit(EXIT_SUCCESS);				 
			case STOP:
                delete_client(message);
                break;              
		}	
	}
}