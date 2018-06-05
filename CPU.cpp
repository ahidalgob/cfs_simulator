#include "CPU.h"
#include "waitqueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <unistd.h>
#include "balancer.h"

CPU::CPU(void* balancer, long long int tick){
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
	time_delta=tick;
	idleq = balancer;
	pthread_mutex_unlock(&sync_mutex);
}

void* CPU::tick_fair(void* arg){
	CPU *cpu = (CPU*) arg;
	usleep(500);
	printf("[fair] [%d] tick_fair OK\n", cpu->number);

	while(true){
		usleep(cpu->time_delta);
		if(!(cpu->busy)){
			cpu->cfs_rq.lock();
			if (cpu->cfs_rq.empty()) {
				cpu->cfs_rq.unlock();
				printf("[fair] [%d] cpu is idle\n", cpu->number);
				continue;
			}
			cpu->running = cpu->cfs_rq.pop_min();
			cpu->cfs_rq.unlock();
			printf("[fair] CPU[%d] is now busy\n", cpu->number);
			printf("[fair] task id %03d has just entered the CPU[%d] with v_runtime %lld ms\n", cpu->running.id, cpu->number, cpu->running.v_runtime);
			cpu->busy = 1;
		}


		cpu->running.v_runtime += floor((cpu->time_delta/1000)*(pow(1.25, cpu->running.nice)));

		printf("[fair] task id %03d has been running in CPU[%d] for v_runtime %lld ms\n", cpu->running.id, cpu->number, cpu->running.v_runtime);

		int goto_io = rand()%200; //(la probabilidad queda entre 0 y 0.5)

		if (goto_io <= cpu->running.io_prob){
			printf("[fair] task id %03d has just exited the CPU[%d] to do i/o (v_runtime %lld ms)\n", cpu->running.id, cpu->number, cpu->running.v_runtime);

            ((BALANCER*)(cpu->idleq))->push_to_idle(cpu->running);

			if (cpu->cfs_rq.empty()){
				printf("[fair] RBT[%d] is empty\n", cpu->number);
				cpu->busy = 0;
			}
			else{
				cpu->running = cpu->cfs_rq.pop_min();
				printf("[fair] task id %03d has just entered the CPU[%d] with v_runtime %lld ms\n", cpu->running.id, cpu->number, cpu->running.v_runtime);
			}
			cpu->cfs_rq.unlock();
            continue;
		}

		cpu->cfs_rq.lock();
		if (cpu->cfs_rq.empty()) {
			//printf("[fair] RBT[%d] is empty\n", cpu->number);
            // No esta vacia, solo esta la que esta running.
		}
		else if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime()){
			cpu->rbt_queue_push(cpu->running);
			cpu->running = cpu->cfs_rq.pop_min();
			printf("[fair] task id %03d has just entered the CPU[%d] with v_runtime %lld ms\n", cpu->running.id, cpu->number, cpu->running.v_runtime);
		}
		cpu->cfs_rq.unlock();
	}
	return(NULL);
}

void* CPU::pusher(void* arg){
	CPU *cpu = (CPU*) arg;
	usleep(500);
	TASK task;
	printf("[pshr] [%d] pusher OK\n", cpu->number);
	while(1){
		sem_wait(&cpu->rbt_queue_sem);
		cpu->rbt_queue_lock();
		if(!cpu->rbt_queue_empty()){
			task = cpu->rbt_queue_pop();
			cpu->rbt_queue_unlock();

			task.last_cpu = cpu->number;

			cpu->cfs_rq.lock();
			cpu->cfs_rq.insert(task);
			cpu->cfs_rq.unlock();
		}
		else cpu->rbt_queue_unlock();
		printf("[pshr] task with id %d and v_runtime %lld ms is in RBT[%d]\n", task.id, task.v_runtime, cpu->number);
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

vector<TASK> CPU::task_list(){
    return cfs_rq.rq_list();
}
