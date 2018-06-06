#include "CPU.h"
#include "waitqueue.h"
#include "console.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <unistd.h>
#include "balancer.h"

CPU::CPU(void* balancer, long long int tick)
{
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

void* CPU::tick_fair(void* arg)
{
	CPU *cpu = (CPU*) arg;
	usleep(500);

	while(true)
	{
		usleep(cpu->time_delta);

		if(!(cpu->busy))
		{
			cpu->cfs_rq.lock();
			if (cpu->cfs_rq.empty()) 
			{
				cpu->cfs_rq.unlock();
				print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
				print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
				print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
				continue;
			}
			cpu->running = cpu->cfs_rq.pop_min();
			print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
			print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
			print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
			cpu->cfs_rq.unlock();
	    	print_running(cpu->number, cpu->running.id, cpu->running.nice, cpu->running.v_runtime);
			cpu->busy = 1;
		}

		cpu->running.v_runtime += floor((cpu->time_delta/1000)*(pow(1.25, cpu->running.nice)));
    	print_running(cpu->number, cpu->running.id, cpu->running.nice, cpu->running.v_runtime);

		int goto_io = rand()%200; //(la probabilidad queda entre 0 y 0.5)

		if (goto_io <= cpu->running.io_prob)
		{
            ((BALANCER*)(cpu->idleq))->push_to_idle(cpu->running);

			if (cpu->cfs_rq.empty()){
				print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
				print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
				print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
				cpu->busy = 0;
			}
			else{
				cpu->running = cpu->cfs_rq.pop_min();
				print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
				print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
				print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
		    	print_running(cpu->number, cpu->running.id, cpu->running.nice, cpu->running.v_runtime);
			}
			cpu->cfs_rq.unlock();
            continue;
		}

		cpu->cfs_rq.lock();
		if (cpu->cfs_rq.empty()) 
		{
			print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
			print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
			print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
		}
		else if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime()){
			cpu->rbt_queue_push(cpu->running);
			cpu->running = cpu->cfs_rq.pop_min();
			print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
			print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
			print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
	    	print_running(cpu->number, cpu->running.id, cpu->running.nice, cpu->running.v_runtime);
		}
		cpu->cfs_rq.unlock();
	}
	return(NULL);
}

void* CPU::pusher(void* arg){
	CPU *cpu = (CPU*) arg;
	usleep(500);
	TASK task;

	while(1)
	{
		sem_wait(&cpu->rbt_queue_sem);
		cpu->rbt_queue_lock();
		if(!cpu->rbt_queue_empty())
		{
			task = cpu->rbt_queue_pop();
			cpu->rbt_queue_unlock();

			task.last_cpu = cpu->number;

			cpu->cfs_rq.lock();
			cpu->cfs_rq.insert(task);
			print_ready3(cpu->number,cpu->cfs_rq.get_3id(), cpu->cfs_rq.get_3v_runtime(), cpu->cfs_rq.size());
			print_ready2(cpu->number,cpu->cfs_rq.get_2id(), cpu->cfs_rq.get_2v_runtime());
			print_ready(cpu->number,cpu->cfs_rq.get_id(), cpu->cfs_rq.get_v_runtime());
			cpu->cfs_rq.unlock();
		}
		else cpu->rbt_queue_unlock();
	}
	return(NULL);
}

int CPU::size(){
    if(!busy) return 0;
    return cfs_rq.size()+1;
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