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
#include <time.h>
#include <sys/ipc.h>
#include "barbershop.h"

int C; //Clients to create (number)
int S; //Number of cuts
int sharedM, semID, barberInvites;
barbershop_state* bState; 
pid_t childPid;
int myID; //ID of a process

void init_shared(); //Initiating shared memory and semaphores
void print_err_ext_failure(char* mes); //Ending program with perror
void funAtExit(void){ shmdt(bState); }
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
	
	if((sharedM = shmget(ftok(SS_PATH,0), 0, 0))<0)
		print_err_ext_failure("Error during shmget client");
	
	if ((bState = (barbershop_state*)shmat(sharedM, NULL, 0)) < 0) 
		print_err_ext_failure("Error during shmat client");
	
	if ((semID = semget(ftok(SS_PATH, 1), 0, 0)) < 0) 
		print_err_ext_failure("Error during semget client");
	
	if ((barberInvites = semget(ftok(SS_PATH, 2), 0, 0)) < 0) 
		print_err_ext_failure("Error during semget client");
    
}

void clientProc(int cutsNr) {

	print_action("Client comes to life, PID:", getpid());	
	struct sembuf sbuf;
	sbuf.sem_flg = 0;
	while(cutsNr >0) {
		
		WAIT(semID, WRSeats)			
		WAIT(semID, BarberSleeps)		
		
		if(bState->nr_of_free_seats > 0) {

			if(bState->barberSleeps) {				
				bState->barberSleeps = false;
				bState->clientThatCame.pid = getpid();				
				SIGNAL(semID, WRSeats)			
				SIGNAL(semID, BarberSleeps)
				SIGNAL(semID, CustReady)				
			}
			else {
				print_action("I'm sitting in a waiting room. Client PID:", getpid());
				(bState->nr_of_free_seats)--;				
				myID = enqueue(&(bState->fifo), getpid());				
				SIGNAL(semID, WRSeats)			
				SIGNAL(semID, BarberSleeps)
				SIGNAL(semID, CustReady)
				WAIT(barberInvites, myID)				
				WAIT(semID, WRSeats)
				(bState->nr_of_free_seats)++;	
				SIGNAL(semID, WRSeats)
			}
			char mes[200];
			sprintf(mes, "I have %d cuts left, and I'm sitting on a barbershop seat. Client PID:", cutsNr);
			print_action(mes, getpid());			
			SIGNAL(semID, BSSeat)
			WAIT(semID, CutDone)
			cutsNr--;
			print_action("I'm leaving the barbershop, client PID:", getpid());
		}
		else {
			print_action("I'm leaving the barbershop, because there are no free seats client PID:", getpid());
			SIGNAL(semID, BarberSleeps)
			SIGNAL(semID, WRSeats)
			int r = rand() % 100;
			usleep(3000 * r);
			//sleep(2);
		}		
	}
	print_action("Client disappears, PID:", getpid());	
	exit(EXIT_SUCCESS);
}