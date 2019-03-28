#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <values.h>
#include <time.h>
#include "clientUtil.h"


void print_err_ext_failure(char * message);
void funAtExit(void);
void sigIntHandler(int signo);
void send_msg(uint8_t message_type, int op, double val);
void calc(void);
void init(char *arg1, char *arg2, char *arg3);

int scket;
enum connect_type c_type;
char *name;


int main(int argc, char **argv) {
/*
	Klient przyjmuje jako swoje argumenty:
	>argv[1] - swoja nazwe (string o z gory ograniczonej dlugosci)
	>argv[2] - sposob polaczenia z serwerem (siec lub komunikacja lokalna przez gniazda UNIX)
	>argv[3] - adres serwera (adres IPv4 i numer portu lub sciezke do gniazda UNIX serwera)
     w formule x.x.x.x:yyyy
*/

    if (argc != 4) {
        print_err_ext_failure("Wrong arguments!");                
    }
    if (atexit(funAtExit) == -1)
        print_err_ext_failure("Client - atexit\n");

	signal(SIGINT, sigIntHandler);

    init(argv[1], argv[2], argv[3]);

    send_msg(LOGIN, 0, 0);

    uint8_t message_type;
    if(read(scket, &message_type, 1) != 1)
        print_err_ext_failure("Client - read response type");
    switch(message_type){
        case WRONG_NAME:
            print_err_ext_failure("Client -name is taken\n");
        case WRONG_SIZE:
            print_err_ext_failure("Client - max client reached\n");
        case SUCCESS:
            printf("Logged in\n");
            break;
        default:
            printf("%d\n",message_type);
            print_err_ext_failure("Client - register default\n");
    }
	//Reading msg loop
    uint8_t msg_type;
    while(1){
        if(read(scket, &msg_type, 1) != 1)
            print_err_ext_failure("Client - read mess type\n");

        switch(msg_type){
            case REQUEST:
                calc();
                break;
            case PING:
                send_msg(PONG, 0, 0);
                break;
            default:
                printf("Client - unknown mess type\n");
                break;
        }
    }

    return 0;
}

void print_err_ext_failure(char * message) {
	perror(message);
	exit(EXIT_FAILURE);
}

void funAtExit(void) {
    send_msg(LOGOUT, 0, 0);
    if (shutdown(scket, SHUT_RDWR) == -1)
        fprintf(stderr, "Client - shutdown\n");
    if (close(scket) == -1)
        fprintf(stderr, "Client - close\n");
}


void init(char *arg1, char *arg2, char *arg3) {
        
    name = arg1;

    switch((strcmp(arg2, "WEB") == 0 ? WEB : strcmp(arg2, "LOCAL") == 0 ? LOCAL : -1)){
        case WEB: {
            strtok(arg3, ":");
            char *arg4 = strtok(NULL, ":");
            if(arg4 == NULL)
                print_err_ext_failure("Client - IP\n");

            uint32_t ip = inet_addr(arg3);
            if (ip == -1)
                print_err_ext_failure("Client - IP address\n");

            uint16_t port_num = (uint16_t) strtol(arg4, NULL, 10);
            if (port_num < 1024 || port_num > 65535)
                print_err_ext_failure("Client - port number\n");

            if ((scket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
                print_err_ext_failure("Client - web socket\n");

            struct sockaddr_in web_address;
            web_address.sin_family = AF_INET;
            web_address.sin_addr.s_addr = htonl(INADDR_ANY);
            web_address.sin_port = 0;

            if (connect(scket, (const struct sockaddr *) &web_address, sizeof(web_address)) == -1)
                print_err_ext_failure("Client - connect to web socket\n");

            web_address.sin_family = AF_INET;
            web_address.sin_addr.s_addr = ip;
            web_address.sin_port = htons(port_num);
            if (connect(scket, (const struct sockaddr *) &web_address, sizeof(web_address)) == -1)
                print_err_ext_failure("Client - bind to web socket\n");

            c_type = WEB;
            break;
        }
        case LOCAL: {
            if((scket = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) print_err_ext_failure("Unix socket");

            struct sockaddr_un unix_addr;
            memset(&unix_addr, 0, sizeof(struct sockaddr_un));
            unix_addr.sun_family = AF_UNIX;
            strncpy(unix_addr.sun_path, arg3,  sizeof(unix_addr.sun_path));

            if (bind(scket, (struct sockaddr*) &unix_addr,
                        sizeof(sa_family_t))<0) print_err_ext_failure("Unix autobind");
            if (connect(scket, (struct sockaddr*) &unix_addr,
                        sizeof(struct sockaddr_un))<0) print_err_ext_failure("Unix connect");
            c_type = LOCAL;
            break;

        }
        default:
            print_err_ext_failure("Client - init default");
    }
}
void sigIntHandler(int signo) {
    print_err_ext_failure("Server: SIGINT received - exit failure");
}

void send_msg(uint8_t message_type, int op, double val){
    message_t msg;
    msg.msg_type = (enum message_type) message_type;
    snprintf(msg.name, 64, "%s", name);
    msg.connect_type = c_type;
    msg.op = op;
    if(val == MINDOUBLE)
        msg.wrong_flag = 1;
    else {
        msg.val = val;
        msg.wrong_flag = 0;
    }
    if(write(scket, &msg, sizeof(message_t)) != sizeof(message_t))
        print_err_ext_failure("Client - write mes\n");

}

void calc(void){

    operation_t operation;
    
    if(read(scket, &operation, sizeof(operation_t)) != sizeof(operation_t))
        print_err_ext_failure("Client - read request msg\n");

    int op = operation.operation;
    
    double res = 0;
    switch (operation.operand)
    {
        case '+':
            res = operation.arg1 + operation.arg2;
            break;
        case '-':
            res = operation.arg1 - operation.arg2;
            break;
        case '/':
            if (operation.arg2) {
                res = (operation.arg1 / operation.arg2);
                break;
            }
            else {
                res = MINDOUBLE;
                break;
            }
        case '*':
            res = operation.arg1 * operation.arg2;
            break;
        default:
            break;
    }

    send_msg(RESULT, op, res);
}
