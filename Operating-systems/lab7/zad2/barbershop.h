#ifndef BARBERSHOP
#define BARBERSHOP

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define S_ALLPERMISSIONS 0000777

#define BARBERSHOP_SHMEM "/barbershop"

#define SEM_BarberSleeps "/BarberSleeps"
#define SEM_CustReady "/CustReady"
#define SEM_WRSeats "/WRSeats"
#define SEM_BSSeat "/BSSeat"
#define SEM_CutDone "/CutDone"

#define MAX_WR_SEATS 100

#include <stdbool.h>

typedef struct {
	int id;
	int pid;
} client;

typedef struct {

	int front, end;
	int size;
	int n;
	client queue[MAX_WR_SEATS];

} fifo_Q;

void init_Q(fifo_Q *qPtr, int N) {
	for(int i=0; i < MAX_WR_SEATS; i++) {
		qPtr->queue[i].id = 0;
		qPtr->queue[i].pid = 0;
	}	
	qPtr->front = qPtr->end = 0;
	qPtr->size = N;
}
int enqueue(fifo_Q *qPtr, int elPid){	

	if(qPtr->n == 0) {		

		(qPtr->n)++;		
		qPtr->queue[qPtr->front].id = qPtr->front;			
		qPtr->queue[qPtr->front].pid = elPid;	
		
		return qPtr->front;	
	}
	else {
		(qPtr->n)++;
		qPtr->end = (qPtr->end+1)%(qPtr->size);
		qPtr->queue[qPtr->end].id = qPtr->end;
		qPtr->queue[qPtr->end].pid = elPid;		
		return qPtr->end;
	}	
}
client dequeue(fifo_Q *qPtr) {	
	
	if(qPtr->n <=1){
		(qPtr->n)--;
		return qPtr->queue[qPtr->front];
	}
	else {
		(qPtr->n)--;
		client retVal;
		retVal.id = qPtr->queue[qPtr->front].id;
		retVal.pid = qPtr->queue[qPtr->front].pid;		
		qPtr->front = (qPtr->front+1)%(qPtr->size);
		return retVal;
	}	
}
typedef struct {

	int nr_of_free_seats;
	bool barberSleeps;	
	fifo_Q fifo;
	sem_t barberInvites[MAX_WR_SEATS];
	client clientThatCame;

} barbershop_state;

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};


#define WAIT(semPtr) sem_wait(semPtr);

#define SIGNAL(semPtr) sem_post(semPtr);

#endif