#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>


volatile sig_atomic_t signal_received = 0;

void handler_STP(int sigNr) {

	if(signal_received == 0){
		printf("....Odebrano sygnal SIGSTP\n");
		printf("Oczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
	}
	if(signal_received)
		signal_received = 0;
	else
		signal_received = 1;
	
}
void handler_INT(int sigNr) {
	printf("....Odebrano sygnał SIGINT\n");
	exit(EXIT_SUCCESS);
}


void print_time() {
	time_t timeVal;
	struct tm * time_info;
	time(&timeVal);
	time_info = localtime(&timeVal);
	printf("Aktualny czas: %s", asctime(time_info));
}

int main(void) {
	struct sigaction action;
	action.sa_handler = handler_STP;
	sigemptyset(&action.sa_mask);
	action.sa_flags =  0;

	

	for(;;) {        
		sigaction(SIGTSTP, &action, NULL);
		signal(SIGINT, handler_INT);
		
        if (!signal_received) {
            print_time();
        }
        sleep(1);
    }

}