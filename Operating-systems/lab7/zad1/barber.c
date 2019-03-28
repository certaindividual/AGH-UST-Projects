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
#include<string.h>
#include "barbershop.h"

int N, sharedM, semID, barberInvites;
barbershop_state* bState;

void init_shared(); //Initiating shared memory and semaphores
void print_err_ext_failure(char* mes); //Ending program with perror
void print_action(char *mes, int pid); //Printing what is happening in the barbershop
void terminateFunction() { exit(EXIT_SUCCESS); }
void funAtExit();

int main(int argc, char const *argv[]) {

	if(argc <2) {
        printf("Please give the number of seats in a barbershop\n");
        exit(EXIT_FAILURE);
    }	
	N = atoi(argv[1]);
	if(N > MAX_WR_SEATS) {
		printf("Max waiting room seats is: %d\n",MAX_WR_SEATS);
		exit(EXIT_FAILURE);
	}
	srand(time(NULL));
	atexit(funAtExit);
	signal(SIGTERM, terminateFunction);
    signal(SIGINT, terminateFunction);

	init_shared();

	client clientToCut;

	while(true) {
		struct sembuf sbuf;
		sbuf.sem_flg = 0;		
		WAIT(semID, WRSeats)		
		WAIT(semID, BarberSleeps)			
		if(bState->nr_of_free_seats == N){
				
			print_action("Barber: I'm going to sleep",-1);
			bState->barberSleeps = true;			
			SIGNAL(semID,WRSeats)
			SIGNAL(semID, BarberSleeps)
			WAIT(semID,CustReady)
			print_action("I was woke up by client PID:", bState->clientThatCame.pid);
			WAIT(semID, BSSeat)
			print_action("Starting a cut on client PID:", bState->clientThatCame.pid);
			int r = rand() % 10;
			usleep(30000 * r);			
			//sleep(1);
			print_action("The cut is done on client PID:", bState->clientThatCame.pid);
			SIGNAL(semID, CutDone)
		}
		else {
			
			clientToCut = dequeue(&(bState->fifo));			
			print_action("Barber: I'm inviting client PID:", clientToCut.pid);			
			SIGNAL(semID,WRSeats)	
			SIGNAL(semID, BarberSleeps)
			WAIT(semID,CustReady)
			SIGNAL(barberInvites, clientToCut.id)
			WAIT(semID, BSSeat)
			print_action("Starting a cut on client PID:", clientToCut.pid);
			int r = rand() % 10;
			usleep(30000 * r);
			//sleep(1);
			print_action("The cut is done on client PID:", clientToCut.pid);
			SIGNAL(semID, CutDone)
		}				
	}
}

void init_shared() {
	if((sharedM = shmget(ftok(SS_PATH,0), sizeof(barbershop_state), IPC_CREAT | S_ALLPERMISSIONS))<0)
		print_err_ext_failure("Error during shmget barber");
	
	if ((bState = (barbershop_state*)shmat(sharedM, NULL, 0)) < 0) 
		print_err_ext_failure("Error during shmat barber");

	bState->nr_of_free_seats = N;
	bState->barberSleeps = false;	
	init_Q(&(bState->fifo), N);
	
	if ((semID = semget(ftok(SS_PATH, 1), 5, IPC_CREAT | S_ALLPERMISSIONS)) < 0) 
		print_err_ext_failure("Error during semget barber");
	
	union semun arg;
	arg.val = 1;
	semctl(semID, WRSeats, SETVAL, arg);
	arg.val = 1;
	semctl(semID, BarberSleeps, SETVAL, arg);

	arg.val = 0;
	int semaphores[] = {CustReady, BSSeat, CutDone};
	for(int semaphore_i=0; semaphore_i <3; semaphore_i++)
		semctl(semID, semaphores[semaphore_i], SETVAL, arg);
	
	if ((barberInvites = semget(ftok(SS_PATH, 2), N, IPC_CREAT | S_ALLPERMISSIONS)) < 0) 
		print_err_ext_failure("Error during semget barber");
	
	for (int i = 0; i < N; i++) semctl(barberInvites, i, SETVAL, arg);

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
		printf("%s%s  \t\t time: %s\n%s", ANSI_COLOR_YELLOW, mes, buf, ANSI_COLOR_RESET);
		fflush(stdout);
	}
	else {
		printf("%s%s%d  \t\t time: %s\n%s", ANSI_COLOR_YELLOW, mes, pid, buf, ANSI_COLOR_RESET);		
		fflush(stdout);
	}
}
void funAtExit() {
	printf(ANSI_COLOR_RED "Closing sems and shmem" ANSI_COLOR_RESET);
	printf("\n");
	shmdt(bState);
    shmctl(sharedM, IPC_RMID, NULL);
    semctl(semID, 0, IPC_RMID, NULL);
    semctl(barberInvites, 0, IPC_RMID, NULL);
}

