#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SIMPLEMODE 0
#define DESCMODE 1

#define MORE_MODE 0
#define LESS_MODE 1
#define EQUAL_MODE 2


int p, k, n, l, nk;
char *book; FILE *bookfile;
int mode, mode_L;
char **buffer = NULL;

int writeTo=0;
int readFrom=0;

int count = 0;
pthread_mutex_t msgs_in_buffer_count = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffIsEmpty_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffIsFull_cv = PTHREAD_COND_INITIALIZER;


void load_conf(char* path);
void print_err_ext_failure(char * message);
void sigexit_handler(int signo);
void* producer_procedure(void* x);
void* consumer_procedure(void* x);
void funAtExit(void);

int main(int argc, char *argv[]) {

	/*
	Program przyjmuje w argumentie wywolania:
	>nazwe pliku konfiguracyjnego
	*/
	if(argc <2) {
        printf("No argument error\n");
        exit(EXIT_FAILURE);
	}
	load_conf(argv[1]);
	signal(SIGINT, sigexit_handler);
	atexit(funAtExit);
	
	buffer = (char**)calloc(n, sizeof(char*));
	
	pthread_t* producers = (pthread_t*)calloc(p, sizeof(pthread_t));
	for (int i = 0; i<p; i++) if (pthread_create(&producers[i], NULL, &producer_procedure, NULL)) print_err_ext_failure("Error during thread creation");

    pthread_t* consumers = (pthread_t*)calloc(k, sizeof(pthread_t));	
    for (int i = 0; i<k; i++) if (pthread_create(&consumers[i], NULL, &consumer_procedure, NULL)) print_err_ext_failure("Error during thread creation");

	if (nk) alarm(nk);

	for (int i = 0; i<p; i++) if (pthread_join(producers[i], NULL)) print_err_ext_failure("Error thread producers join");
	
	for(;;) {

        pthread_mutex_lock(&msgs_in_buffer_count);
        if (count == 0) exit(EXIT_SUCCESS);
        pthread_mutex_unlock(&msgs_in_buffer_count);
    }   

}
void* producer_procedure(void* x) {

	char* buff = NULL;

    while (true) {

		size_t ns = 0;	
		usleep((int)(rand() % 5));
        pthread_mutex_lock(&msgs_in_buffer_count);

        while (count >= n)
            pthread_cond_wait(&buffIsFull_cv, &msgs_in_buffer_count);

        if (getline(&buff, &ns, bookfile) <= 0) {
            pthread_mutex_unlock(&msgs_in_buffer_count);
            pthread_exit((void*) 0);
        }
				

		if (mode == DESCMODE) {			
			printf(ANSI_COLOR_GREEN "Producer ID: %d putting a msg into: %d  (%d/%d)\n" ANSI_COLOR_RESET, (int)pthread_self(), writeTo, count+1, n);
			printf("%s\n", buff);			
		}		
		
		free(buffer[writeTo]);
		buffer[writeTo] = (char*)calloc(strlen(buff), sizeof(char));		
		
		if(strlen(buff)>0){
			for(int i=0;i<strlen(buff);i++)
				buffer[writeTo][i] = buff[i];
		}			
		        
		count++;
        writeTo = (writeTo+1)%n;
        
        pthread_cond_signal(&buffIsEmpty_cv);
        pthread_mutex_unlock(&msgs_in_buffer_count);    
		
        	
    }    

}
void* consumer_procedure(void* x) {	
    
    while (true) {
		
		usleep((int)(rand() % 5));
        pthread_mutex_lock(&msgs_in_buffer_count);

        while(count <= 0)
            pthread_cond_wait(&buffIsEmpty_cv, &msgs_in_buffer_count);
		
		if (mode == DESCMODE) printf(ANSI_COLOR_MAGENTA "Consumer ID: %d reading a msg from: %d  (%d/%d)\n" ANSI_COLOR_RESET, (int)pthread_self(), readFrom, count-1, n);

		if(strlen(buffer[readFrom]) > 0) {
						

			if(mode_L == MORE_MODE){
				if(strlen(buffer[readFrom]) > l) printf(ANSI_COLOR_YELLOW "Consumer ID: %d read from buffer[%d]: %s" ANSI_COLOR_RESET, (int)pthread_self(), readFrom, buffer[readFrom]);
			}
			else if(mode_L == LESS_MODE) {
				if(strlen(buffer[readFrom]) < l) printf(ANSI_COLOR_YELLOW "Consumer ID: %d read from buffer[%d]: %s" ANSI_COLOR_RESET, (int)pthread_self(), readFrom, buffer[readFrom]);
			}
			else {
				if(strlen(buffer[readFrom]) == l) printf(ANSI_COLOR_YELLOW "Consumer ID: %d read from buffer[%d]: %s" ANSI_COLOR_RESET, (int)pthread_self(), readFrom, buffer[readFrom]);
			}				
			
		}				        

        readFrom = (readFrom+1)%n;
        count--;		

        pthread_mutex_unlock(&msgs_in_buffer_count);        
		pthread_cond_signal(&buffIsFull_cv);
    }    

}

void load_conf(char* path) {

	/*
	Example config file:
		P= 10
		K= 10
		N= 20
		book.txt
		L= 25
		lmode= 1
		mode= DESC
		NK= 0
	*/

	FILE *file;
	if((file = fopen(path, "r"))==NULL) print_err_ext_failure("Error incorrect config filename");
	char delims[] = {' ', '\n', '\0'};
	size_t nt = 0;

	char* p_line = NULL;
	if(0 == getline(&p_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *p_eq = strtok(p_line, delims);
	if(strcmp(p_eq, "P=")!=0) print_err_ext_failure("Incorrect config file");
	p = atoi(strtok(NULL, delims));

	char* k_line = NULL;
	if(0 == getline(&k_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *k_eq = strtok(k_line, delims);
	if(strcmp(k_eq, "K=")!=0) print_err_ext_failure("Incorrect config file");
	k = atoi(strtok(NULL, delims));

	char* n_line = NULL;
	if(0 == getline(&n_line,&nt, file)) print_err_ext_failure("Error reading from file");	
	char *n_eq = strtok(n_line, delims);
	if(strcmp(n_eq, "N=")!=0) print_err_ext_failure("Incorrect config file");
	n = atoi(strtok(NULL, delims));	

	char *book;
	if(0 == getline(&book, &nt, file)) print_err_ext_failure("Error reading from file");
	if (book[strlen(book)-1] == '\n') book[strlen(book)-1] = '\0';
	if((bookfile = fopen(book, "r"))==NULL) print_err_ext_failure("Error incorrect source filename");

	char* l_line = NULL;
	if(0 == getline(&l_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *l_eq = strtok(l_line, delims);	
	if(strcmp(l_eq, "L=")!=0) print_err_ext_failure("Incorrect config file");
	l = atoi(strtok(NULL, delims));
	

	char* mode_L_line = NULL;
	if(0 == getline(&mode_L_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *mode_L_eq = strtok(mode_L_line, delims);
	if(strcmp(mode_L_eq, "lmode=")!=0) print_err_ext_failure("Incorrect config file");
	mode_L = atoi(strtok(NULL, delims));
	if(mode_L != 0 && mode_L !=1 && mode_L  != 2) print_err_ext_failure("Incorrect config file");

	char* mode_line = NULL;
	if(0 == getline(&mode_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *mode_eq = strtok(mode_line, delims);
	if(strcmp(mode_eq, "mode=")!=0) print_err_ext_failure("Incorrect config file");
	mode = atoi(strtok(NULL, delims));
	if(mode != 0 && mode !=1) print_err_ext_failure("Incorrect config file");

	char* nk_line = NULL;
	if(0 == getline(&nk_line,&nt, file)) print_err_ext_failure("Error reading from file");
	char *nk_eq = strtok(nk_line, delims);
	if(strcmp(nk_eq, "NK=")!=0) print_err_ext_failure("Incorrect config file");
	nk = atoi(strtok(NULL, delims));

}

void print_err_ext_failure(char * message) {
	perror(message);
	exit(EXIT_FAILURE);
}
void sigexit_handler(int signo){

	printf(ANSI_COLOR_RED "CTRL + C : The end\n" ANSI_COLOR_RESET);
	exit(EXIT_SUCCESS);
}

void funAtExit() {	
	
    if (buffer) free(buffer);
    if (bookfile) fclose(bookfile);
    pthread_mutex_destroy(&msgs_in_buffer_count);
    pthread_cond_destroy(&buffIsEmpty_cv);
    pthread_cond_destroy(&buffIsFull_cv);

}