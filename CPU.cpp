#include "CPU.h"
#include "waitqueue.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#define time_delta 5000000 //microsegundos
#define ioprob 25 // X% de probabilidad de i/o

CPU::CPU(){
	idle_queue.resize(WAITQUEUE_N);

	sem_init(&rbt_queue_sem, 0, 0);
	pthread_mutex_init(&rbt_queue_mutex, NULL);

	pthread_t fair_thread;
	pthread_create(&fair_thread, NULL, (&tick_fair), (void*) this);
	pthread_detach(fair_thread);

	pthread_t idle_thread;
	pthread_create(&idle_thread, NULL, (&tick_idle), (void*) this);
	pthread_detach(idle_thread);

	pthread_t pusher_thread;
	pthread_create(&pusher_thread, NULL, (&pusher), (void*) this);
	pthread_detach(pusher_thread);

	busy=0;
}

void* CPU::tick_fair(void* arg){
	CPU *cpu = (CPU*) arg;
	printf("[fair] start\n");

	while(true){
		if(!(cpu->busy)){	
			if (cpu->cfs_rq.empty()) {
				printf("[fair] cpu is idle\n");
				usleep(time_delta);
				continue;
			}
			cpu->cfs_rq.lock();
			cpu->running = cpu->cfs_rq.get_min();
			cpu->cfs_rq.unlock();

			printf("[fair] cpu is busy\n");
			cpu->busy = 1;
		}


		
		
		usleep(time_delta);

		cpu->running.v_runtime += time_delta;

		printf("[fair] process %d has been running for %lld useconds\n", cpu->running.id, cpu->running.v_runtime);
		
		/*
		int goto_io = rand()%(10000/ioprob); //(la probabilidad queda entre 0 y 0.25)

		if (goto_io <= cpu->running.io_prob)
		{
			cpu->idle_queue[rand()%WAITQUEUE_N].push(cpu->running);
			cpu->cfs_rq.lock();
			cpu->running = cpu->cfs_rq.get_min();
			cpu->cfs_rq.erase_min();
			cpu->cfs_rq.unlock();
		}
		*/

		cpu->cfs_rq.lock();
		
		printf("[fair] CPU v_runtime: %lld, min_vruntime: %lld\n",cpu->running.v_runtime, cpu->cfs_rq.get_min_v_runtime());
		
		if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime()){
			cpu->rbt_queue_push(cpu->running);
			cpu->running = cpu->cfs_rq.get_min();
		}
		cpu->cfs_rq.unlock();
	}
	return(NULL);
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

void* CPU::pusher(void* arg){
	CPU *cpu = (CPU*) arg;
	TASK task;
	printf("[pusher] start\n");
	while(1){
		sem_wait(&cpu->rbt_queue_sem);

		if(!cpu->rbt_queue_empty()){
			task = cpu->rbt_queue_pop();
			cpu->cfs_rq.lock();
			cpu->cfs_rq.insert(task);
			cpu->cfs_rq.unlock();
		}
		printf("[pusher] task with id %d is in the RBT\n", task.id);
	}
	return(NULL);
}


void CPU::rbt_queue_push(TASK &task){
	pthread_mutex_lock(&rbt_queue_mutex);
	rbt_queue.push(task);
	pthread_mutex_unlock(&rbt_queue_mutex);
	sem_post(&rbt_queue_sem);
}

TASK CPU::rbt_queue_pop(){
	pthread_mutex_lock(&rbt_queue_mutex);
	TASK task = rbt_queue.front();
	rbt_queue.pop();
	pthread_mutex_unlock(&rbt_queue_mutex);

	return task;
}

bool CPU::rbt_queue_empty(){
	bool e;
	pthread_mutex_lock(&rbt_queue_mutex);
	e = rbt_queue.empty();
	pthread_mutex_unlock(&rbt_queue_mutex);
	return e;
}
