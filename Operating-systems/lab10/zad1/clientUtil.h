#define UNIX_PATH_MAX 108
#define CLIENT_MAX 8
#define PING_TIME 10

typedef struct Client {
    int fd;
    char *name;
    uint8_t is_inactive;
} Client;

typedef enum connect_type {
    LOCAL,
    WEB
} connect_type;

typedef struct result_t {
    int wrong_flag;
    int operation;
    double value;
} result_t;

typedef struct operation_t {
    int operation;
    char operand;
    double arg1;
    double arg2;
} operation_t;


typedef enum message_type {
    LOGIN,
    LOGOUT,
    SUCCESS,
    WRONG_SIZE,
    WRONG_NAME,
    REQUEST,
    RESULT,
    PING,
    PONG,
} message_type;



