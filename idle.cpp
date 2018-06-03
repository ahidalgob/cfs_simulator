#include "idle.h"
#include "waitqueue.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#define time_delta 1000000 //microsegundos
#define iofinished 25 // X% de probabilidad de terminar i/o
#define WAITQUEUE_N 4 // Number of idle queues (i/o device types)

IDLE::IDLE(){
	waitqueues.resize(WAITQUEUE_N);

	pthread_t idle_thread;
	pthread_create(&idle_thread, NULL, (&tick_idle), (void*) this);
	pthread_detach(idle_thread);
}

void push_to_idle(TASK &task){
	int nqueue = rand()%WAITQUEUE_N;
	pthread_mutex_lock(&idle_queue_mutex);
	waitqueues[nqueue].push(task);
	pthread_mutex_unlock(&idle_queue_mutex);
}

void* CPU::tick_idle(void* arg){
	/*
	printf("idle start\n");
	CPU *cpu = (CPU*) arg;
	while(true){
		printf("tick idle\n");
		usleep(time_delta);

		for(int i=0; i<WAITQUEUE_N; i++){
			if(cpu->idle_queue[i].empty()) continue;
			int out_prob = rand()%200;
			if(out_prob <= cpu->idle_queue[i].idle_prob){
				TASK tsk = cpu->idle_queue[i].pop();
				cpu->rbt_queue_push(tsk);
				printf("iddle pop!\n");
			}
		}

	}
	*/
	return(NULL);
}