#include "task.h"
#include "CPU.h"
#include "waitqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define maxntask 5

using namespace std;

int main()
{
	srand (time(NULL));
	int ntask = 1;
	CPU cpu;
	while(ntask <= maxntask){
		TASK task = *(new TASK());
		task.id = ntask;
		ntask++;
		printf("[main] sent task with id %d, nice=%d, io_prob=%d to pusher\n", task.id, task.nice, task.io_prob);
		cpu.rbt_queue_push(task);
	}
	while(1){};	

	

	return 0;
}
