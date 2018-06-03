#include "balancer.h"
#include "waitqueue.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#define time_delta 1000000 //microsegundos
#define iofinished 25 // X% de probabilidad de terminar i/o
#define WAITQUEUE_N 2
#define CPU_N 1


BALANCER::BALANCER(){
	int nqueue = 0;
	int ncpu = 0;

	while(nqueue < WAITQUEUE_N){
		WAITQUEUE *wq = new WAITQUEUE();
		waitqueues.push_back(*wq);
		printf("[blcr] created idle queue number %d with idle_prob=%d\n", nqueue, wq->idle_prob);
		nqueue++;
		delete wq;
	}

	while(ncpu < CPU_N){
		printf("[blcr] creating CPU[%d]\n", ncpu);
		CPU *proc = new CPU;
		proc->number = ncpu;
		cpu.push_back(*proc);
		ncpu++;
		delete proc;
	}

	pthread_t idle_thread;
	pthread_create(&idle_thread, NULL, (&tick_idle), (void*) this);
	pthread_detach(idle_thread);
}

void BALANCER::push_to_idle(TASK &task){
	int nqueue = rand()%WAITQUEUE_N;
	waitqueues[nqueue].push(task);
}

void* BALANCER::tick_idle(void* arg){
	/*
	printf("[idle] start\n");
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