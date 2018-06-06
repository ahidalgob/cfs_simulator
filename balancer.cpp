#include "balancer.h"
#include "waitqueue.h"
#include "console.h"

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>


BALANCER::BALANCER(int CPU_N, int WAITQUEUE_N, int tick){
	int nqueue = 0;
	int ncpu = 0;
	waitqueue_n = WAITQUEUE_N;
	time_delta = tick;

	// Iniciar pantalla
    console_init();

	while(ncpu < CPU_N)
	{
		cpu.push_back(new CPU(this, time_delta));
		cpu[ncpu]->number = ncpu;
		//printf("[blcr] initializing CPU[%d]\n", cpu[ncpu]->number);
		ncpu++;
	}

	while(nqueue < WAITQUEUE_N){
		wqs.push_back(new WAITQUEUE());
		//printf("[blcr] created idle queue number %d with idle_prob=%d\n", nqueue, wqs[nqueue]->idle_prob);
		nqueue++;
	}

	pthread_t idle_thread;
	pthread_create(&idle_thread, NULL, (&tick_idle), (void*) this);
	pthread_detach(idle_thread);

}

void BALANCER::push_to_idle(TASK &task){
	int nqueue = rand()%waitqueue_n;
	wqs[nqueue]->push(task);
	print_queues(nqueue, wqs[nqueue]->get_id(), wqs[nqueue]->size(), wqs[nqueue]->get_last());
}

void* BALANCER::tick_idle(void* arg){
	BALANCER *balancer = (BALANCER*) arg;
	//printf("[idle] idle queues OK\n");
	while(true){
	//	printf("[idle] tick idle\n");
		usleep(balancer->time_delta);

		for(int i=0; i < balancer->waitqueue_n; i++)
		{
			if(balancer->wqs[i]->empty())
			{	print_queues(i, balancer->wqs[i]->get_id(), balancer->wqs[i]->size(), balancer->wqs[i]->get_last());
				continue;
			}
			int out_prob = rand()%100;
			if(out_prob <= balancer->wqs[i]->idle_prob)
			{
				TASK tsk = balancer->wqs[i]->pop();
                int mn_sz = balancer->cpu[0]->size(), mn_id=0;

                for(int j=1; j<(int)balancer->cpu.size(); j++)
                {
                    //mn_sz = min(mn_sz, balancer->cpu[j]->size());
                    //mn_id = j;
                	if(balancer->cpu[j]->size() < mn_sz)
                	{
                        mn_sz = balancer->cpu[j]->size();
                        mn_id = j;
                	}
                }
                if(mn_sz*10 <= balancer->cpu[tsk.last_cpu]->size())
                {
                    tsk.last_cpu = mn_id;
                }
				balancer->cpu[tsk.last_cpu]->rbt_queue_push(tsk);
				//printf("[idle] Queue no. %d popped task %03d\n", i, tsk.id);
			}
			print_queues(i, balancer->wqs[i]->get_id(), balancer->wqs[i]->size(), balancer->wqs[i]->get_last());
		}
	}
	return(NULL);
}
