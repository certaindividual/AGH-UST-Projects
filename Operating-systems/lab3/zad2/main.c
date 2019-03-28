#include <stdio.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_ARGS 64
#define MAX_LINE_LENGTH 256

pid_t vfork(void);

enum arguments_ret_val {DONE, NO_MORE_PARAMS, ERR};

void print_line(char **args, int *args_count) {
	for (int i = 0; i < (*args_count); i++) {
    	printf("%s ", args[i]);
	}
}

enum arguments_ret_val get_arguments(char *line, char **args, FILE *file, int *args_count ) {
	
	(*args_count) = 0;
	char * s = fgets(line, MAX_LINE_LENGTH, file);		
	if(s == NULL) return NO_MORE_PARAMS;	
	char *strtok_arg = ((*args_count) == 0 ? line : NULL);	
	while((args[(*args_count)] = strtok(strtok_arg, " \n")) != NULL) {
		
		if(++(*args_count) >= MAX_ARGS) {
			printf("Zbyt wiele argumentow\n");
			print_line(args, args_count);
			return ERR;
		}	
		strtok_arg = (args_count == 0 ? line : NULL);	
	}
	return DONE;
}


int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Zbyt mala liczba argumentow");
        return 1;
    }
    FILE* file;
    if ((file = fopen(argv[1], "r")) == NULL) {
        perror("Niepoprawna nazwa pliku ");
		printf("\n");
        return 1;
    }
    char *line = (char*) malloc(MAX_LINE_LENGTH*sizeof(char));
    char **args = (char**) malloc(MAX_ARGS*sizeof(char*));
	int *args_count = (int*) malloc(sizeof(int));

	//Getting args for the first time
	enum arguments_ret_val flag = get_arguments(line, args, file, args_count);	
	if(flag == ERR) return 1;
	
	while(flag == DONE) {
		
		
		pid_t pid_flag = vfork();
        if(pid_flag == 0) {			
            execvp(args[0], args);			
            exit(EXIT_FAILURE);
        }
        int * stat = (int*) malloc(sizeof(int));
        wait(stat);
        if ((*stat)) {
            printf( "Blad podczas wykonywania:\n");
            print_line(args, args_count);
            return 1;
        }

		flag = get_arguments(line, args, file, args_count);
		if(flag == ERR) return 1;
	}

    fclose(file);
    return 0;
}