#define _BSD_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#define MAX_ARGS 64
#define MAX_LINE_LENGTH 256


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
    if(argc < 4) {
        printf("Zbyt mala liczba argumentow");
        return 1;
    }
    FILE* file;
    if ((file = fopen(argv[1], "r")) == NULL) {
        perror("Niepoprawna nazwa pliku ");
		printf("\n");
        return 1;
    }
	char *time_limit = malloc(strlen(argv[2]) * sizeof(char*));
	char *memory_limit = malloc(strlen(argv[3]) * sizeof(char*));
	strcpy(time_limit, argv[2]);
	strcpy(memory_limit, argv[3]);

	struct rusage start;
    getrusage(RUSAGE_CHILDREN, &start);

    char *line = (char*) malloc(MAX_LINE_LENGTH*sizeof(char));
    char **args = (char**) malloc(MAX_ARGS*sizeof(char*));
	int *args_count = (int*) malloc(sizeof(int));

	//Getting args for the first time
	enum arguments_ret_val flag = get_arguments(line, args, file, args_count);	
	if(flag == ERR) return 1;
	
	while(flag == DONE) {		
		
		pid_t pid_flag = vfork();
        if(pid_flag == 0) {			
			long time_limit_l = strtol(time_limit, NULL, 10);
			long memory_limit_l = strtol(memory_limit, NULL, 10) * 1048576;
			struct rlimit rlimit_time, rlimit_memory;
			rlimit_time.rlim_max =  (rlimit_time.rlim_cur = (rlim_t)time_limit_l);
			rlimit_memory.rlim_max = (rlimit_memory.rlim_cur = (rlim_t)memory_limit_l);
			
			if (setrlimit(RLIMIT_CPU, &rlimit_time) != 0) {
				perror("Blad przy nakladaniu limitu na czas:");
				printf("\n");
				return 1;
			}       

			if (setrlimit(RLIMIT_AS, &rlimit_memory) != 0) {
				perror("Blad przy nakladaniu limitu na pamiec:");
				printf("\n");
				return 1;
			}
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


		struct rusage end;
		getrusage(RUSAGE_CHILDREN, &end);
		struct timeval user_time;
		struct timeval system_time;
		timersub(&end.ru_utime, &start.ru_utime, &user_time);
		timersub(&end.ru_stime, &start.ru_stime, &system_time);
		start = end;
		printf("-------------------------------------------------\n");
		print_line(args, args_count);
		
		printf("\n");
		printf("Czas uzytkownika: %d.%d s\n", (int) user_time.tv_sec, (int) user_time.tv_usec);
		printf("Czas systemowy: %d.%d s\n", (int) system_time.tv_sec, (int) system_time.tv_usec);

		flag = get_arguments(line, args, file, args_count);
		if(flag == ERR) return 1;
	}

    fclose(file);
    return 0;
}