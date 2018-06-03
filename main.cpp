#include "balancer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define maxntask 5	// Max number of tasks

using namespace std;

int main()
{
	srand (time(NULL));
	int ntask = 1, ncpu=0;
	BALANCER balancer;
	while(ntask <= maxntask){
		TASK *task = new TASK();
		task->id = ntask;
		ntask++;
		printf("[main] sent id %03d, nice=%d to pusher[%d]\n", task->id, task->nice, ncpu);
		balancer.cpu[0].rbt_queue_push(*task);
		delete task;
	}
	while(1){};	

	

	return 0;
}
