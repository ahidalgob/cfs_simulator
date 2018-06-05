#include "balancer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define maxntask 5			// Numero inicial de tasks

#define time_delta 1000000	// microsegundos

#define ioprob 25			// X% de probabilidad de i/o

#define iofinished 25		// X% de probabilidad de terminar i/o

#define WAITQUEUE_N 2		// Numero de colas de procesos bloqueados (i/o)

#define CPU_N 1				// Cambiar el número de procesadores no
							// se reflejará adecuadamente en la CLI

using namespace std;

int main()
{
	srand (time(NULL));
	int ntask = 1;
	BALANCER balancer(CPU_N, WAITQUEUE_N, time_delta);

	
	while(ntask <= maxntask){
		TASK *task = new TASK();
		task->id = ntask;
		ntask++;
		printf("[main] sent id %03d, nice=%d to a random pusher\n", task->id, task->nice);
		balancer.cpu[ntask%CPU_N]->rbt_queue_push(*task);
		delete task;
	}
	

	while(1){};	

	

	return 0;
}
