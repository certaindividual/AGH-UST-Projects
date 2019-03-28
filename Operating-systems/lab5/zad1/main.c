#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_ARGS_NR 64
#define MAX_LINE_LENGTH 256
#define MAX_SUBCOMMANDS 5

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

pid_t pid;
int stdout_save;
int read_pos = 0;

typedef enum MyRetVal {DONE, NO_MORE_LINES, ERR} MyRetVal;

void print_line(char ***args, int *args_count, int nr) {
	for (int i = 0; i < args_count[nr]; i++) {
    	printf("%s %s %s", ANSI_COLOR_RED, args[nr][i], ANSI_COLOR_RESET);
	}
	printf("\n");
}

void print_process_info(char ***args, int *args_count, int nr){	
	
	int	prev_save = dup(STDOUT_FILENO);
	dup2(stdout_save, STDOUT_FILENO);
	printf(ANSI_COLOR_YELLOW     "Proces o PID: %d wykona polecenie:"     ANSI_COLOR_RESET "\n", getpid());
	print_line(args, args_count, nr);
	dup2(prev_save, STDOUT_FILENO);
}

void extract_subcommands(char *line, char **subcommands) {
	int n=0;
	int pos=0;
	int i=0;
	subcommands[n] = (char*)calloc(MAX_LINE_LENGTH,sizeof(char));
	while((line[i]!='\n') && (line[i] != '\0')){
		
		if(line[i]=='|'){
						
			subcommands[n++][pos] = '\0';
			subcommands[n] = (char*)calloc(MAX_LINE_LENGTH,sizeof(char));	
			pos = 0;			
		}		
		else {			
			subcommands[n][pos++] = line[i];
		}
		i++;
	}	
	subcommands[n][pos] = '\0';	
	return;
}

MyRetVal get_pipeline(int *args_count, char*** args, FILE *file) {

	char *line = (char*) calloc(MAX_LINE_LENGTH, sizeof(char));   

	char ** subcommands = (char**)calloc(MAX_SUBCOMMANDS,sizeof(char*));

	for(int i=0;i<MAX_SUBCOMMANDS;i++) args_count[i] = 0;
	
	char * flag = fgets(line, MAX_LINE_LENGTH, file);
	
	if(flag == NULL) return NO_MORE_LINES;	
	
	printf("--------------------------------------------\n");
	printf("%s Polecenie do wykonania: \n %s %s", ANSI_COLOR_MAGENTA, line, ANSI_COLOR_RESET);
	printf("--------------------------------------------\n");

	extract_subcommands((char*)line, subcommands);	
	
	for(int i=0; i < MAX_SUBCOMMANDS && subcommands[i]!= NULL; i++) {		

		args[i] = (char**) calloc(MAX_ARGS_NR, sizeof(char*));
		while( ( args[i][args_count[i]] = strtok(args_count[i] == 0 ? subcommands[i] : NULL, " \n\0") ) != NULL  ) {
			
			if(++args_count[i] >= MAX_ARGS_NR) {
				printf("Zbyt wiele argumentow linii polecen\n");
				print_line(args, args_count, i);
				return ERR;
			}			
		}
	}	
	free(line);
	return DONE;

}

void reset_vars(int *args_count, char*** args) {
	
	for(int i=0;i<MAX_SUBCOMMANDS;i++) args_count[i] = 0;
	
	for(int i=0; i < MAX_SUBCOMMANDS;i++) {		
		args[i] = NULL;				
	}	
}

void exec_recursion(char ***args, int i, int *args_count ) {
	
	if(i<0) return;
	
	int fd[2];
	pipe(fd);
	pid = fork();
	
	if (pid == 0) { // dziecko		
		if(i!=0){
			close(fd[0]); 
			dup2(fd[1],STDOUT_FILENO);		
			
			exec_recursion(args, i-1, args_count);
		}
		else {					
			exit(EXIT_SUCCESS);	
		}
	} else { // rodzic		
		
		close(fd[1]);
		dup2(fd[0],STDIN_FILENO);					
		int stat;
		wait(&stat);
		if (stat) {
			printf( "Blad podczas wykonywania:\n");
			print_line(args, args_count, i-1);			
			exit(EXIT_FAILURE);
		}
		print_process_info(args,args_count,i);
						
		execvp(args[i][0], args[i]);		
        exit(EXIT_FAILURE);
	}
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

	stdout_save = dup(STDOUT_FILENO);
	
	//Liczebnosc argumentow poszczegolnych subpolecen
	int *args_count = (int*) calloc(MAX_SUBCOMMANDS, sizeof(int));

	//Argumenty poszczegolnych subpolecen z jednej linii
	//Tablica dwuwymiarowa stringow (char*)	
	char ***args = (char***)calloc(MAX_SUBCOMMANDS, sizeof(char**));
	
	MyRetVal flag;
	//Pobranie polecen z pierwszej linii
	
	if((flag = get_pipeline(args_count, (char***)args, file)) == ERR) return 1;
	read_pos = ftell(file);
	int start = 0;
	while(args[start+1]!=NULL) start++;
			
	while(flag == DONE) {		
		
		pid = fork();
        if(pid == 0) {//Child	
			
			exec_recursion(args, start, args_count);			
			exit(EXIT_FAILURE);
        }
		else {//Parent
		
			int stat;
			waitpid(pid,&stat,0);
			
			if (stat) {
				printf( "Blad podczas wykonywania:\n");
				print_line(args, args_count, 0);				
				return 1;
			}
			printf("%sProces rodzic PID: %d: Polecenie wykonane\n %s", ANSI_COLOR_GREEN, getpid(), ANSI_COLOR_RESET);					
					
		}		
        reset_vars(args_count, args);
		
		file = fopen(argv[1], "r");
		fseek(file,read_pos,SEEK_SET);
		if((flag = get_pipeline(args_count, (char***)args, file)) == ERR) return 1;	
		read_pos = ftell(file);
		
		start = 0;
		while(args[start+1]!=NULL) start++;
		
	}
    fclose(file);
    return 0;
}
