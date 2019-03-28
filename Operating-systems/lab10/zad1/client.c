#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include "clientUtil.h"


void print_err_ext_failure(char * message);
void funAtExit(void);
void sigIntHandler(int signo);
void send_msg(uint8_t message_type);
void calc(void);
void init(char *arg1, char *arg2, char *arg3);

int scket;
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
        print_err_ext_failure("Client - atexit error");

	signal(SIGINT, sigIntHandler);

    init(argv[1], argv[2], argv[3]);

    send_msg(LOGIN);

    uint8_t message_type;
    if(read(scket, &message_type, 1) != 1)
        print_err_ext_failure("Client - error during read response");
    switch(message_type){
        case WRONG_NAME:
            print_err_ext_failure("Client -name is taken");
        case WRONG_SIZE:
            print_err_ext_failure("Client - max client reached");
        case SUCCESS:
            printf("Logged in\n");
            break;
        default:
            print_err_ext_failure("Client - register default");
    }
	//Reading msg loop
    uint8_t msg_type;
    while(1){
        if(read(scket, &msg_type, 1) != 1)
            print_err_ext_failure("Client - read mess type");

        switch(msg_type){
            case REQUEST:
                calc();
                break;
            case PING:
                send_msg(PONG);
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
    send_msg(LOGOUT);
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

            struct sockaddr_in web_address;
            web_address.sin_family = AF_INET;
            web_address.sin_addr.s_addr = ip;
            web_address.sin_port = htons(port_num);

            if ((scket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                print_err_ext_failure("Client - web socket\n");

            if (connect(scket, (const struct sockaddr *) &web_address, sizeof(web_address)) == -1)
                print_err_ext_failure("Client - connect to web socket\n");

            break;
        }
        case LOCAL: {
            char* unix_path = arg3;
            if (strlen(unix_path) < 1 || strlen(unix_path) > UNIX_PATH_MAX)
                print_err_ext_failure("Client - unix path\n");

            struct sockaddr_un local_address;
            local_address.sun_family = AF_UNIX;
            snprintf(local_address.sun_path, UNIX_PATH_MAX, "%s", unix_path);
            int tmp1 = (scket = socket(AF_UNIX, SOCK_STREAM, 0));
            if (tmp1 < 0)
                print_err_ext_failure("Client - local socket\n");
            int tmp2 = connect(scket, (const struct sockaddr *) &local_address, sizeof(local_address));

            if (tmp2 == -1)
                print_err_ext_failure("Client - connect to local socket\n");

            break;
        }
        default:
            print_err_ext_failure("Client - init default");
    }

}
void sigIntHandler(int signo) {
    print_err_ext_failure("Server: SIGINT received - exit failure");
}

void send_msg(uint8_t message_type){
    uint16_t message_size = (uint16_t) (strlen(name) + 1);
    if(write(scket, &message_type, 1) != 1)
        print_err_ext_failure("Client - write mes type");
    if(write(scket, &message_size, 2) != 2)
        print_err_ext_failure("Client - write mes size");
    if(write(scket, name, message_size) != message_size)
        print_err_ext_failure("Client - write mes name");
}

void calc(void){

    operation_t operation;
    result_t result;

    if(read(scket, &operation, sizeof(operation_t)) != sizeof(operation_t))
        print_err_ext_failure("Client - read request msg");

    result.operation = operation.operation;
    result.value = 0;
    result.wrong_flag = 0;
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
                result.wrong_flag = 1;
                break;
            }
        case '*':
            res = operation.arg1 * operation.arg2;
            break;
        default:
            result.wrong_flag = 1;
            break;
    }
    result.value = res;


    send_msg(RESULT);
    if(write(scket, &result, sizeof(result_t)) != sizeof(result_t))
        print_err_ext_failure("Client - write result\n");
}
