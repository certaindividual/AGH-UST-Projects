#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>



pid_t pid;

void run_shellscript() {	
	execl("./skrypcik.sh", "skrypcik.sh", NULL);//path, nazwa programu, null konczacy liste argumentow
	perror("Blad dotyczacy skryptu");
	exit(EXIT_FAILURE);
}


void handler_STP(int sigNr) {	
	printf("....Odebrano sygnal SIGSTP\n");
	if (kill(pid, 0) < 0)//dziecko nie istnieje
	{		
		if ((pid = fork()) < 0){
			perror("Blad funkcji fork ");
			exit(EXIT_FAILURE);
		}
		if (pid == 0)//wewnatrz dziecka
		{
			run_shellscript();
		}
	
	}
	else //dziecko istnieje - zabicie procesu i wyswietlenie komunikatu
	{	
		if(kill(pid,SIGKILL) == 0){
			waitpid(pid, NULL,0);
		}		
		printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu.\n");
	}	
}
void handler_INT(int sigNr) {
	if(pid==0) return;
	printf("....Odebrano sygnał SIGINT\n");	
	
	if(kill(pid,SIGKILL) == 0){
		waitpid(pid, NULL,0);
	}	
	exit(EXIT_SUCCESS);	
}

int main(void) {
	struct sigaction action;
	action.sa_handler = handler_STP;
	sigemptyset(&action.sa_mask);
	action.sa_flags =  0;

	sigaction(SIGTSTP, &action, NULL);
	signal(SIGINT, handler_INT);	

	if ((pid = fork()) < 0)	{
		perror("Blad funkcji fork ");
		exit(EXIT_FAILURE);
	}

	if (pid == 0)//wewnatrz dziecka
	{
		run_shellscript();
	}

	for(;;) {
        pause();
    }
}