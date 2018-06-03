#include "balancer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define maxntask 5	// Max number of tasks
#define CPU_N 2


using namespace std;

int main()
{
	srand (time(NULL));
	int ntask = 1;
	BALANCER balancer;

	
	while(ntask <= maxntask){
		TASK *task = new TASK();
		task->id = ntask;
		ntask++;
		printf("[main] sent id %03d, nice=%d to a random pusher\n", task->id, task->nice);
		balancer.cpu[ntask%CPU_N].rbt_queue_push(*task);
		delete task;
	}
	

	while(1){};	

	

	return 0;
}
