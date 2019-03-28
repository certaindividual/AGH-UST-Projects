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
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <sys/ipc.h>
#include <string.h>
#include "barbershop.h"


int N, sharedM;
barbershop_state* bState;

sem_t* BarberSleeps;
sem_t* CustReady;
sem_t* WRSeats;
sem_t* BSSeat;
sem_t* CutDone;

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
				
		WAIT(WRSeats)		
		WAIT(BarberSleeps)			
		if(bState->nr_of_free_seats == N){
				
			print_action("Barber: I'm going to sleep",-1);
			bState->barberSleeps = true;			
			SIGNAL(WRSeats)
			SIGNAL(BarberSleeps)
			WAIT(CustReady)
			print_action("I was woke up by client PID:", bState->clientThatCame.pid);
			WAIT(BSSeat)
			print_action("Starting a cut on client PID:", bState->clientThatCame.pid);
			int r = rand() % 10;
			usleep(30000 * r);			
			//sleep(1);
			print_action("The cut is done on client PID:", bState->clientThatCame.pid);
			SIGNAL(CutDone)
		}
		else {
			
			clientToCut = dequeue(&(bState->fifo));			
			print_action("Barber: I'm inviting client PID:", clientToCut.pid);			
			SIGNAL(WRSeats)	
			SIGNAL(BarberSleeps)
			WAIT(CustReady)
			SIGNAL(&((bState->barberInvites)[clientToCut.id]))
			WAIT(BSSeat)
			print_action("Starting a cut on client PID:", clientToCut.pid);
			int r = rand() % 10;
			usleep(30000 * r);
			//sleep(1);
			print_action("The cut is done on client PID:", clientToCut.pid);
			SIGNAL(CutDone)
		}				
	}
}

void init_shared() {

	if((sharedM = shm_open(BARBERSHOP_SHMEM, O_RDWR | O_CREAT, S_ALLPERMISSIONS))<0)
		print_err_ext_failure("Error during shm_open barber");
		
	if (ftruncate(sharedM, sizeof(barbershop_state)) < 0) 
		print_err_ext_failure("Error during ftruncate barber");

	if ((bState = (barbershop_state*)mmap(NULL, sizeof(barbershop_state), PROT_READ | PROT_WRITE, MAP_SHARED, sharedM, 0)) < 0) 
		print_err_ext_failure("Error during mmap barber");

	close(sharedM);

	bState->nr_of_free_seats = N;
	bState->barberSleeps = false;	
	init_Q(&(bState->fifo), N);
	
	BarberSleeps = sem_open(SEM_BarberSleeps, O_CREAT, S_ALLPERMISSIONS, 1);
	if(BarberSleeps == SEM_FAILED) print_err_ext_failure("Error during sem_open barber");
	
	CustReady = sem_open(SEM_CustReady, O_CREAT, S_ALLPERMISSIONS, 0);
	if(CustReady == SEM_FAILED) print_err_ext_failure("Error during sem_open barber");
	
	WRSeats = sem_open(SEM_WRSeats, O_CREAT, S_ALLPERMISSIONS, 1);
	if(WRSeats == SEM_FAILED) print_err_ext_failure("Error during sem_open barber");
	
	BSSeat = sem_open(SEM_BSSeat, O_CREAT, S_ALLPERMISSIONS, 0);
	if(BSSeat == SEM_FAILED) print_err_ext_failure("Error during sem_open barber");
	
	CutDone = sem_open(SEM_CutDone, O_CREAT, S_ALLPERMISSIONS, 0);
	if(CutDone == SEM_FAILED) print_err_ext_failure("Error during sem_open barber");	
	
	for (int i = 0; i < N; i++) sem_init(&(bState->barberInvites[i]), 1, 0);

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

	for(int i = 0; i<MAX_WR_SEATS; i++) {
		if(sem_destroy((bState->barberInvites)+i) < 0) {
			perror("Error during sem_destroy");
		}	
	} 
	
	if(munmap((void*)bState, sizeof(barbershop_state))<0) perror("Error during munmap");

	if(shm_unlink(BARBERSHOP_SHMEM)<0) perror("Error during shm_unlink");

	if(sem_close(BarberSleeps)<0) perror("Error during sem_close (BarberSleeps)");
	if(sem_unlink(SEM_BarberSleeps)<0) perror("Error during sem_unlink (SEM_BarberSleeps)");

	if(sem_close(CustReady)<0) perror("Error during sem_close (CustReady)");
	if(sem_unlink(SEM_CustReady)<0) perror("Error during sem_unlink (SEM_CustReady)");	

	if(sem_close(WRSeats)<0) perror("Error during sem_close (WRSeats)");
	if(sem_unlink(SEM_WRSeats)<0) perror("Error during sem_unlink (SEM_WRSeats)");		

	if(sem_close(BSSeat)<0) perror("Error during sem_close (BSSeat)");
	if(sem_unlink(SEM_BSSeat)<0) perror("Error during sem_unlink (SEM_BSSeat)");	
	
	if(sem_close(CutDone)<0) perror("Error during sem_close (CutDone)");
	if(sem_unlink(SEM_CutDone)<0) perror("Error during sem_unlink (SEM_CutDone)");			
	
}
