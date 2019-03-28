#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include "clientUtil.h"


pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
Client clients[CLIENT_MAX];
int client_counter = 0;
int operation_counter = 0;

int web_socket;
int local_socket;
int epoll;
char *unix_pathname;

pthread_t pingThread;
pthread_t terminalThread;

void funAtExit(void);
void sigIntHandler(int signo);
void print_err_ext_failure(char * message);
void init_sockets(char* argv1, char* argv2);
void init_epoll(void);
void *ping_procedure (void *);
void *terminal_procedure (void *);
void logoutClient(char *client_name);
void login_client(int socket, message_t msg, struct sockaddr *sockaddr, socklen_t socklen);
void message(int socket);
void remove_client(int i);
int check_name(const char *name);


int main(int argc, char **argv){
	/*
	Program serwera przyjmuje w argumentie wywolania:
	>argv[1] - numer portu TCP
	>argv[2] - sciezke gniazda UNIX
	*/
	if(argc <3) {
        printf("Wrong arguments error\n");
        exit(EXIT_FAILURE);
	}
	signal(SIGINT, sigIntHandler);
	atexit(funAtExit);

	init_sockets(argv[1], argv[2]);
	init_epoll();

    if (pthread_create(&pingThread, NULL, &ping_procedure, NULL) != 0)
		print_err_ext_failure("Error during ping thread creation");

    if (pthread_create(&terminalThread, NULL, &terminal_procedure, NULL) != 0)
		print_err_ext_failure("Error during terminal thread creation");

    struct epoll_event event;
        
    while (1) {
        if (epoll_wait(epoll, &event, 1, -1) == -1)
            print_err_ext_failure("Server - epoll wait\n");
        message(event.data.fd);
    }
}
 
void init_sockets(char* argv1, char* argv2) {
	
	//WEB SOCKET INIT
	//Numer portu TCP (argv[1])
    uint16_t port_nr = (uint16_t) atoi(argv1);
    if (port_nr <= 1023 || port_nr >= 65536) print_err_ext_failure("Incorrect port number");

	//Web Socket
    struct sockaddr_in socketAddr_web;
	socketAddr_web.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddr_web.sin_family = AF_INET;    
    socketAddr_web.sin_port = htons(port_nr);
    

    if ((web_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        print_err_ext_failure("Error during socket web creation");
    
    if (bind(web_socket, (const struct sockaddr*) &socketAddr_web, sizeof(socketAddr_web)))
		print_err_ext_failure("Error during binding web socket");


	//LOCAL SOCKET INIT
	//Sciezka gniazda UNIX	(argv[2])    
    unix_pathname = argv2;
    if (strlen(unix_pathname) <= 0 || strlen(unix_pathname) > UNIX_PATH_MAX)
        print_err_ext_failure("Incorrect Unix path");
    
	//Local Socket
    struct sockaddr_un socketAddr_local;
    memset(&socketAddr_local, 0, sizeof(struct sockaddr_un));
    socketAddr_local.sun_family = AF_UNIX;

    sprintf(socketAddr_local.sun_path, "%s", unix_pathname);

    if ((local_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
        print_err_ext_failure("Error during socket local creation");    

    if (bind(local_socket, (const struct sockaddr*) &socketAddr_local, sizeof(socketAddr_local)))
        print_err_ext_failure("Error during binding local socket");

}
void init_epoll(void) {
		
    if ((epoll = epoll_create1(0)) == -1)
        print_err_ext_failure("Error during epoll creation");

	struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;

    event.data.fd = web_socket;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, web_socket, &event) == -1)
        print_err_ext_failure("Error during adding web socket to epoll");

    event.data.fd = local_socket;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, local_socket, &event) == -1)
        print_err_ext_failure("Error during adding local socket to epoll");

}

void funAtExit(void) {
    pthread_cancel(pingThread);
    pthread_cancel(terminalThread);
    if (close(web_socket) == -1)
        fprintf(stderr, "Server - closing web socket");
    if (close(local_socket) == -1)
        fprintf(stderr, "Server - closing local socket");
    if (unlink(unix_pathname) == -1)
        fprintf(stderr, "Server - unlinking unix path");
    if (close(epoll) == -1)
        fprintf(stderr, "Server - closing epoll");
}

void print_err_ext_failure(char * message) {
	perror(message);
	exit(EXIT_FAILURE);
}

void sigIntHandler(int signo) {
    print_err_ext_failure("Server: SIGINT received - exit failure");
}



void message(int socket) {
    struct sockaddr* sockaddr = malloc(sizeof(struct sockaddr));
    socklen_t socklen = sizeof(struct sockaddr);
    message_t msg;

    if(recvfrom(socket, &msg, sizeof(message_t), 0, sockaddr, &socklen) != sizeof(message_t))
        print_err_ext_failure("Server - read msg type\n");


    switch(msg.msg_type){
        case LOGIN:{
            login_client(socket, msg, sockaddr, socklen);
            break;
        }
        case LOGOUT:{
            logoutClient(msg.name);
            break;
        }
        case RESULT:{
            if(msg.wrong_flag == 1){
                printf("Error - divide by 0\n");
                break;
            }
            printf("%d:Result: %lf\t\tClient - \"%s\"\n", msg.op, msg.val, msg.name);
            break;
        }
        case PONG:{
            pthread_mutex_lock(&clients_mutex);
            int i = check_name(msg.name);
            if (i >= 0)
                clients[i].is_inactive--;
            pthread_mutex_unlock(&clients_mutex);
            break;
        }
        default:
            printf("Unknown message type\n");
            break;
    }
}

void *ping_procedure(void *arg) {
    uint8_t msg_type = PING;
    while (1) {
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_counter; ++i) {
            if (clients[i].is_inactive != 0) {
                printf("Client \"%s\" is inactive, removing from list.\n", clients[i].name);
                remove_client(i--);
            } else {
                int socket = clients[i].connect_type == WEB ? web_socket : local_socket;
                if (sendto(socket, &msg_type, 1, 0, clients[i].sockaddr, clients[i].socklen) != 1)
                    print_err_ext_failure("Server - cannot ping clients\n");
                clients[i].is_inactive++;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(PING_TIME);
    }
}
void remove_client(int i) {
    free(clients[i].sockaddr);
    free(clients[i].name);


    client_counter--;
    for (int j = i; j < client_counter; ++j)
        clients[j] = clients[j + 1];

}
void logoutClient(char *client_name){
    pthread_mutex_lock(&clients_mutex);
    int i = check_name(client_name);
    if(i >= 0){
        remove_client(i);
        printf("Client \"%s\" logout\n", client_name);
    }
    pthread_mutex_unlock(&clients_mutex);
}
void *terminal_procedure(void *arg) {
    srand((unsigned int) time(NULL));
    operation_t msg;
    uint8_t message_type = REQUEST;
    int error = 0;
    char buffer[256];
    while (1) {
        printf("Enter order: \n");
        fgets(buffer, 256, stdin);
        if (sscanf(buffer, "%lf %c %lf", &msg.arg1, &msg.operand, &msg.arg2) != 3) {
            printf("Wrong format\n");
            printf("Correct format: arg1 <operator> arg2\n");
            continue;
        }
        if (msg.operand != '+' && msg.operand != '-' && msg.operand != '*' && msg.operand != '/') {
            printf("Wrong operator\n");
            printf("Available operators: + - * /\n");
            continue;
        }
        msg.operation = ++operation_counter;
        pthread_mutex_lock(&clients_mutex);
        if(client_counter == 0){
            printf("There are no clients connected\n");
            continue;
        }
        error = 0;
        int i = rand() % client_counter;

        int socket = clients[i].connect_type == WEB ? web_socket : local_socket;

        if (sendto(socket, &message_type, 1, 0, clients[i].sockaddr, clients[i].socklen) != 1) error = 1;
        if (sendto(socket, &msg, sizeof(operation_t), 0, clients[i].sockaddr, clients[i].socklen) !=
            sizeof(operation_t)) error = 1;
        if (error == 0)
            printf("Command %d: %lf %c %lf Has been sent to client \"%s\"\n",
                   msg.operation, msg.arg1, msg.operand, msg.arg2, clients[i].name);
        else
            printf("Cannot send order to \"%s\"\n", clients[i].name);
        pthread_mutex_unlock(&clients_mutex);

    }
}

int check_name(const char *name){
    for (int i = 0; i < client_counter; ++i){
        if (strcmp(clients[i].name,name) == 0) {
            return i;
        }
    }

    return -1;
}

void login_client(int socket, message_t msg, struct sockaddr *sockaddr, socklen_t socklen){
    uint8_t msg_type;
    pthread_mutex_lock(&clients_mutex);
    if(client_counter == CLIENT_MAX){
        msg_type = WRONG_SIZE;
        if (sendto(socket, &msg_type, 1, 0, sockaddr, socklen) != 1)
            print_err_ext_failure("Server - write wrong_size\n");
        free(sockaddr);
    } else {
        int exists = check_name(msg.name);
        if(exists != -1){
            msg_type = WRONG_NAME;
            if (sendto(socket, &msg_type, 1, 0, sockaddr, socklen) != 1)
                print_err_ext_failure("Server - write wrong_name\n");
            free(sockaddr);
        } else {
            clients[client_counter].sockaddr = sockaddr;
            clients[client_counter].connect_type = msg.connect_type;
            clients[client_counter].socklen = socklen;
            clients[client_counter].name = malloc(strlen(msg.name) + 1);
            clients[client_counter].is_inactive = 0;
            strcpy(clients[client_counter++].name, msg.name);
            msg_type = SUCCESS;
            if (sendto(socket, &msg_type, 1, 0, sockaddr, socklen) != 1)
                print_err_ext_failure("Server - write success\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}