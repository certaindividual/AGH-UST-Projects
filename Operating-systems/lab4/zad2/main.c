#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
int N,K,n,k;
pid_t *children, *wait_list;
pid_t pid;

void killEmAll() {
	for (int i = 0; i < N; i++){
		if (children[i] != 0) {
            kill(children[i], SIGKILL);
            waitpid(children[i], NULL, 0);
        }
	}
}

void removeChildFromArr(pid_t pid) {
	for (int i = 0; i < N; i++){
        if (children[i] == pid) {
            children[i] = 0;
            return;
        }
	}
}

void handler_INT(int sig, siginfo_t *info, void *ucontext) {
	if(pid!=0){
		printf("Rodzic: Otrzymano SIGINT\n");
		killEmAll();
		exit(0);			
	}
	
}
void handler_CHLD(int sig, siginfo_t *info, void *ucontext){	

	switch(info->si_code){
		case (CLD_EXITED):
			printf("Dziecko o PID: %d zakonczylo dzialanie ze statusem: %d\n", info->si_pid, info->si_status);
			removeChildFromArr(info->si_status);
			n--;
			break;
		case (CLD_KILLED):
			printf("Dziecko o PID: %d zostalo zabite ze statusem: %d\n", info->si_pid, info->si_status);
			removeChildFromArr(info->si_status);
			n--;
			break;
		case (CLD_DUMPED):
			printf("Dziecko o PID: %d niespodziewanie zakonczylo dzialanie ze statusem: %d\n", info->si_pid, info->si_status);
			removeChildFromArr(info->si_status);
			n--;
			break;
		case (CLD_TRAPPED):
			printf("Dziecko o PID: %d jest TRAPPED ze statusem: %d\n", info->si_pid, info->si_status);
			break;
		case (CLD_STOPPED):
			printf("Dziecko o PID: %d zatrzymane ze statusem: %d\n", info->si_pid, info->si_status);
			break;
		case (CLD_CONTINUED):
			printf("Dziecko o PID: %d wznowilo dzialanie ze statusem: %d\n", info->si_pid, info->si_status);
			break;

	}
	if (n == 0) {
        printf("Rodzic: Brak dzieci. KONIEC\n");
        exit(0);
    }	
    removeChildFromArr(info->si_status);
}
void handler_USR(int sig, siginfo_t *info, void *ucontext){
	if(pid!=0){
		printf("Rodzic: Otrzymano prosbe o rozpoczecie dzialania od dziecka o PID: %d\n", info->si_pid);
		if(k>=K){
			printf("Rodzic: Wydano pozwolenie na dzialanie dziecku o PID: %d\n", info->si_pid);
			kill(info->si_pid, SIGUSR1);
			waitpid(info->si_pid, NULL, 0);
		}
		else{					
			wait_list[k] = info->si_pid;
			k++;
			if(k==K){
				for(int i=0; i<K;i++){
					if(wait_list[i]!=0){
						printf("Rodzic: Wydano pozwolenie na dzialanie dziecku o PID: %d\n", wait_list[i]);
						kill(wait_list[i], SIGUSR1);
						waitpid(wait_list[i], NULL, 0);
					}
				}
			}
		}
	}
	else{
		//Dziecko		
		kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
	}
	
}
void handler_REAL(int sig, siginfo_t *info, void *ucontext){
	printf("Rodzic: Otrzymano SIGRTMIN+%d, od dziecka o PID: %d\n", sig-SIGRTMIN, info->si_pid);
}

int childProcedure(){	
	srand((unsigned)getpid());
	unsigned sleep_interval = (unsigned) (rand() % 11);
	sigset_t newmask, oldmask, waitmask;
	sigfillset(&newmask);
	sigfillset(&waitmask);
	sigdelset(&waitmask, SIGUSR1);	
	printf("Dziecko PID: %d, symulacja pracy przez: %d\n", getpid(),(int)sleep_interval);
	sleep(sleep_interval);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);
	kill(getppid(), SIGUSR1);
	sigsuspend(&waitmask);
	sigprocmask(SIG_SETMASK, &oldmask, NULL);

	return sleep_interval;
}

int main(int argc, char *argv[]) {
	if(argc <3) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
	N = (int) strtol(argv[1], '\0', 10);
    K = (int) strtol(argv[2], '\0', 10);
	n=0; k = 0;

	if(K < 0 || N < 0) {
		printf("K i N musza byc liczbami dodatnimi\n");
        return 1;
	}
	if(K > N){
		printf("K nie moze byc wieksze od N\n");
        return 1;
	}
	children = (pid_t*)calloc(N, sizeof(pid_t));
	wait_list = (pid_t*)calloc(N, sizeof(pid_t));

	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = handler_INT;
	sigemptyset(&action.sa_mask);	
	sigaction(SIGINT, &action, NULL);

	action.sa_sigaction = handler_CHLD;
	sigaction(SIGCHLD, &action, NULL);

	action.sa_sigaction = handler_USR;
	sigaction(SIGUSR1, &action, NULL);

	for (int i = 0; i <= 32; i++) {
        action.sa_sigaction = handler_REAL;
		sigaction(SIGRTMIN + i, &action, NULL);
    }
	for(int i = 0; i<N; i++){	
		
		if ((pid = fork()) < 0)	{
			perror("Blad funkcji fork ");
			exit(EXIT_FAILURE);
		}
		if(pid != 0) {			
			children[n] = pid;
			n++;
		}
		else{
			return childProcedure();
			printf("Niepoprawne wykonanie programu dziecka\n");
			exit(EXIT_FAILURE);
		}

	}
	for(;;) {
        wait(NULL);
    }

}