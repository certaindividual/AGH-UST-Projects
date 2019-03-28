#define _GNU_SOURCE
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <sys/ipc.h>
#include "barbershop.h"


int C; //Clients to create (number)
int S; //Number of cuts
int sharedM;
barbershop_state* bState; 
pid_t childPid;
int myID; //ID of a process

sem_t* BarberSleeps;
sem_t* CustReady;
sem_t* WRSeats;
sem_t* BSSeat;
sem_t* CutDone;

void init_shared(); //Initiating shared memory and semaphores
void print_err_ext_failure(char* mes); //Ending program with perror
void funAtExit(void);
void print_action(char *mes, int pid); //Printing what is happening in the barbershop
void terminateFunction(int signo) { exit(EXIT_SUCCESS); }
void clientProc(int cutsNr); //Client procedure

int main(int argc, char const *argv[]) {

	if(argc <2) {
        printf("Incorrect arguments\n");
        exit(EXIT_FAILURE);
    }
	int C = atoi(argv[1]);
    int S = atoi(argv[2]);
	srand(time(NULL));
	atexit(funAtExit);	
	signal(SIGTERM, terminateFunction);
    signal(SIGINT, terminateFunction);	

	init_shared();

	for(int i=0; i<C;i++) {
		if((childPid = fork()) <0) print_err_ext_failure("Error during fork");	
		if(childPid ==0) clientProc(S);			
	}
	exit(EXIT_SUCCESS);
}

void print_err_ext_failure(char* mes) {
    if (errno) perror(mes);
    else printf(ANSI_COLOR_RED "%s\n" ANSI_COLOR_RESET, mes);
    exit(EXIT_FAILURE);
}

void print_action(char *mes, int pid) {

	struct tm *t;
	char buf[30];

	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);

	t = localtime(&(time.tv_sec));
	sprintf(buf, "%d:%d:%d:%ld", t->tm_hour, t->tm_min, t->tm_sec, time.tv_nsec);

	if(pid==-1) {
		printf("%s%s  \t\t time: %s\n%s", ANSI_COLOR_CYAN, mes, buf, ANSI_COLOR_RESET);
		fflush(stdout);
	}
	else {
		printf("%s%s%d  \t\t time: %s\n%s", ANSI_COLOR_CYAN, mes, pid, buf, ANSI_COLOR_RESET);		
		fflush(stdout);
	}
}

void init_shared() {
	
	if((sharedM = shm_open(BARBERSHOP_SHMEM, O_RDWR, 0))<0)
		print_err_ext_failure("Error during shm_open client");
	
	if ((bState = (barbershop_state*)mmap(NULL, sizeof(barbershop_state), PROT_READ | PROT_WRITE, MAP_SHARED, sharedM, 0)) < 0) 
		print_err_ext_failure("Error during mmap client");
	
	close(sharedM);
	
	BarberSleeps = sem_open(SEM_BarberSleeps, 0);
	CustReady = sem_open(SEM_CustReady, 0);
	WRSeats = sem_open(SEM_WRSeats, 0);
	BSSeat = sem_open(SEM_BSSeat, 0);
	CutDone = sem_open(SEM_CutDone, 0);
    
}

void clientProc(int cutsNr) {

	print_action("Client comes to life, PID:", getpid());		
	while(cutsNr >0) {
		
		WAIT(WRSeats)			
		WAIT(BarberSleeps)		
		
		if(bState->nr_of_free_seats > 0) {

			if(bState->barberSleeps) {				
				bState->barberSleeps = false;
				bState->clientThatCame.pid = getpid();				
				SIGNAL(WRSeats)			
				SIGNAL(BarberSleeps)
				SIGNAL(CustReady)				
			}
			else {
				print_action("I'm sitting in a waiting room. Client PID:", getpid());
				(bState->nr_of_free_seats)--;				
				myID = enqueue(&(bState->fifo), getpid());				
				SIGNAL(WRSeats)			
				SIGNAL(BarberSleeps)
				SIGNAL(CustReady)
				WAIT(&((bState->barberInvites)[myID]))				
				WAIT(WRSeats)
				(bState->nr_of_free_seats)++;	
				SIGNAL(WRSeats)
			}
			char mes[200];
			sprintf(mes, "I have %d cuts left, and I'm sitting on a barbershop seat. Client PID:", cutsNr);
			print_action(mes, getpid());			
			SIGNAL(BSSeat)
			WAIT(CutDone)
			cutsNr--;
			print_action("I'm leaving the barbershop, client PID:", getpid());
		}
		else {
			print_action("I'm leaving the barbershop, because there are no free seats client PID:", getpid());
			SIGNAL(BarberSleeps)
			SIGNAL(WRSeats)
			int r = rand() % 100;
			usleep(3000 * r);
			//sleep(2);
		}		
	}
	print_action("Client disappears, PID:", getpid());	
	exit(EXIT_SUCCESS);
}

void funAtExit(void){ 
	
	munmap(bState, sizeof(barbershop_state));
	sem_close(BarberSleeps);
	sem_close(CustReady);
	sem_close(WRSeats);
	sem_close(BSSeat);
	sem_close(CutDone);
}
