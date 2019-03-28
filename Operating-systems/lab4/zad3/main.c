#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

pid_t pid;
int Type;
int L;
int wyslane_do_potomka = 0;
int odebrane_przez_potomka = 0;
int odebrane_od_potomka = 0;

void handler_INT(int sig, siginfo_t *info, void *ucontext) {
	if(pid!=0){
		printf(".....Rodzic: Otrzymano SIGINT\n");
		if(Type == 1){
			printf("Rodzic: Wysylam sygnal konca pracy do dziecka\n");
			wyslane_do_potomka++;
			kill(pid, SIGUSR2);
		}
		else{
			printf("Rodzic: Wysylam sygnal konca pracy do dziecka\n");
			wyslane_do_potomka++;
			kill(pid, SIGRTMAX);
		}
		exit(0);			
	}	
}

void handler_USR1(int sig, siginfo_t *info, void *ucontext){
	if(pid!=0){
		printf("Rodzic: Otrzymano potwierdzenie dostarczenia sygnalu\n");
		odebrane_od_potomka++;
	}			
	
	else{
		//Dziecko	
		printf("Dziecko: otrzymalem sygnal SIGUSR1 od rodzica\n");
		odebrane_przez_potomka++;
		printf("Dziecko: wysylam potwierdzenie odbioru\n");
		kill(getppid(), SIGUSR1);
	}	
}
void handler_USR2(int sig, siginfo_t *info, void *ucontext){
	printf("Dziecko: koncze swoja prace\n");
	printf("Dziecko: Odebralem %d sygnalow od rodzica\n", odebrane_przez_potomka);
	exit(0);	
}
void handler_REALMIN(int sig, siginfo_t *info, void *ucontext){
	if(pid!=0){
		printf("Rodzic: Otrzymano potwierdzenie dostarczenia sygnalu\n");	
		odebrane_od_potomka++;
	}
	
	else{
		printf("Dziecko: otrzymalem sygnal SIGRTMIN od rodzica\n");
		odebrane_przez_potomka++;
		printf("Dziecko: wysylam potwierdzenie odbioru\n");	
		kill(getppid(), SIGRTMIN);
	}
}
void handler_REALMAX(int sig, siginfo_t *info, void *ucontext){	
	printf("Dziecko: koncze swoja prace\n");
	printf("Odebralem %d sygnalow od rodzica\n", odebrane_przez_potomka);
	exit(0);
}

void childProcedure(){	
	
	sigset_t newmask, oldmask;
	sigfillset(&newmask);	
	sigdelset(&newmask, SIGUSR1);	
	sigdelset(&newmask, SIGUSR2);
	sigdelset(&newmask, SIGRTMIN);	
	sigdelset(&newmask, SIGRTMAX);		

	sigprocmask(SIG_SETMASK, &newmask, &oldmask);

	for(;;){
		pause();
	}		
}

int main(int argc, char *argv[]) {
	if(argc <3) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
	L = (int) strtol(argv[1], '\0', 10);
	Type = (int) strtol(argv[2], '\0', 10);
	
	if(Type <= 0 || Type >3) {
		printf("Niepoprawny argument Type\n");
        return 1;
	}
	if(L <= 0 ) {
		printf("Niepoprawny argument L\n");
        return 1;
	}

	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = handler_INT;
	sigemptyset(&action.sa_mask);	
	sigaction(SIGINT, &action, NULL);

	action.sa_sigaction = handler_USR1;
	sigaction(SIGUSR1, &action, NULL);

	action.sa_sigaction = handler_USR2;
	sigaction(SIGUSR2, &action, NULL);

	action.sa_sigaction = handler_REALMIN;
	sigaction(SIGRTMIN, &action, NULL);

	action.sa_sigaction = handler_REALMAX;
	sigaction(SIGRTMAX, &action, NULL);	
	
		
	if ((pid = fork()) < 0)	{
		perror("Blad funkcji fork ");
		exit(EXIT_FAILURE);
	}
	if(pid != 0) {
		sigset_t waitmask;
		sigfillset(&waitmask);
		sigdelset(&waitmask, SIGUSR1);			
		
		switch (Type){
			case (1):
				for(int i=0;i<L;i++){
					printf("Rodzic: wysylam sygnal SIGUSR1 do dziecka\n");
					wyslane_do_potomka++;
					kill(pid, SIGUSR1);
				}
				printf("Rodzic: wysylam sygnal SIGUSR2 do dziecka\n");
				wyslane_do_potomka++;
				kill(pid, SIGUSR2);
				break;
			case (2):				
				for(int i=0;i<L;i++){
					printf("Rodzic: wysylam sygnal SIGUSR1 do dziecka\n");
					wyslane_do_potomka++;
					kill(pid, SIGUSR1);
					sigsuspend(&waitmask);
				}
				printf("Rodzic: wysylam sygnal SIGUSR2 do dziecka\n");
				wyslane_do_potomka++;
				kill(pid, SIGUSR2);
				break;
			case (3):
				for(int i=0;i<L;i++){
					printf("Rodzic: wysylam sygnal SIGRTMIN do dziecka\n");
					wyslane_do_potomka++;
					kill(pid, SIGRTMIN);
				}
				printf("Rodzic: wysylam sygnal SIGRTMAX do dziecka\n");	
				wyslane_do_potomka++;				
				kill(pid, SIGRTMAX);
				break;
			
		}
		
		printf("Wyslano %d sygnalow do potomka\n", wyslane_do_potomka);		
		printf("Rodzic odebral %d sygnalow od potomka\n", odebrane_od_potomka);
		exit(0);
	}
	else{
		childProcedure();
		printf("Niepoprawne wykonanie programu dziecka\n");
		exit(EXIT_FAILURE);
	}
	
	for(;;) {
        wait(NULL);
    }

}