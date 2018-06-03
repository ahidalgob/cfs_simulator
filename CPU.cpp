#include "CPU.h"
#include "waitqueue.h"
#include "console.h"

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#define time_delta 1000000 //microsegundos
#define ioprob 25 // X% de probabilidad de i/o

CPU::CPU(){
	sem_init(&rbt_queue_sem, 0, 0);
	pthread_mutex_init(&rbt_queue_mutex, NULL);
	pthread_mutex_init(&sync_mutex, NULL);

	pthread_t pusher_thread;
	pthread_create(&pusher_thread, NULL, (&pusher), (void*) this);
	pthread_detach(pusher_thread);

	pthread_t fair_thread;
	pthread_create(&fair_thread, NULL, (&tick_fair), (void*) this);
	pthread_detach(fair_thread);

	busy=0;
	number=-1;
	pthread_mutex_unlock(&sync_mutex);
}

void* CPU::tick_fair(void* arg){	
	CPU *cpu = (CPU*) arg;
	usleep(500);
	printf("[fair] [%d] start\n", cpu->number);

	while(true){
		if(!(cpu->busy)){
			cpu->cfs_rq.lock();
			if (cpu->cfs_rq.empty()) {
				cpu->cfs_rq.unlock();
				/*if (cpu->number == 0)
				{
					lcd_write_at(4, 0, "[%d] cpu is idle", cpu->number);
				}
				else
					lcd_write_at(4, LINES, "[%d] cpu is idle", cpu->number);*/

				printf("[fair] [%d] cpu is idle\n", cpu->number);
				usleep(time_delta);
				continue;
			}
			cpu->running = cpu->cfs_rq.pop_min();
			cpu->cfs_rq.unlock();
			printf("[fair] [%d] cpu is now busy\n", cpu->number);
			printf("[fair] [%d] task id %d has just entered the CPU with v_runtime %lld ms\n", cpu->number, cpu->running.id, cpu->running.v_runtime);
			cpu->busy = 1;
		}
		
		usleep(time_delta);

		cpu->running.v_runtime += time_delta/1000;

		printf("[fair] [%d] task id %d has been running for %lld ms\n", cpu->number, cpu->running.id, cpu->running.v_runtime);
		
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
		if (cpu->cfs_rq.empty()) {
			printf("[fair] [%d] RBT is empty\n", cpu->number);
		}
		else if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime()){
			cpu->rbt_queue_push(cpu->running);
			cpu->running = cpu->cfs_rq.pop_min();
			printf("[fair] [%d] task id %d has just entered the CPU with v_runtime %lld ms\n", cpu->number, cpu->running.id, cpu->running.v_runtime);
		}
		cpu->cfs_rq.unlock();
	}
	return(NULL);
}

void* CPU::pusher(void* arg){
	CPU *cpu = (CPU*) arg;
	usleep(500);
	TASK task;
	printf("[pshr] [%d] start\n", cpu->number);
	while(1){
		sem_wait(&cpu->rbt_queue_sem);
		cpu->rbt_queue_lock();
		if(!cpu->rbt_queue_empty()){	
			task = cpu->rbt_queue_pop();
			cpu->rbt_queue_unlock();
			cpu->cfs_rq.lock();
			cpu->cfs_rq.insert(task);
			cpu->cfs_rq.unlock();
		}
		else cpu->rbt_queue_unlock();
		printf("[pshr] [%d] task with id %d and v_runtime %lld ms is in the RBT\n", cpu->number, task.id, task.v_runtime);
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
	TASK task = rbt_queue.front();
	rbt_queue.pop();
	return task;
}

bool CPU::rbt_queue_empty(){
	bool e;
	e = rbt_queue.empty();
	return e;
}

void CPU::rbt_queue_lock(){
	pthread_mutex_lock(&rbt_queue_mutex);
}

void CPU::rbt_queue_unlock(){
	pthread_mutex_unlock(&rbt_queue_mutex);
}