#ifndef MESSAGEHEADER
#define MESSAGEHEADER

#define MIRROR  1
#define CALC    2
#define TIME    3
#define END     4
#define STOP	5
#define INTRODUCE	6
#define ACCEPT	7
#define REPLY 8
#define ERROR	9


#define MSG_MAXSIZE 30
#define MAX_CLIENTS 10

#define S_ALLPERMISSIONS 0000777

struct msgbuf1 {

    long mtype;
    char mtext[MSG_MAXSIZE];
    int clientId;
    pid_t pid;
};

#endif