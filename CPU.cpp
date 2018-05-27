#include "CPU.h"
#include "waitqueue.h"
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#define time_delta 500000

CPU::CPU()
{

    idle_queue.resize(WAITQUEUE_N);

    sem_init(&rbt_queue_sem, 0, 0);
    pthread_mutex_init(&rbt_queue_mutex, NULL);


    pthread_t fair_thread;
    pthread_create(&fair_thread, NULL, (&tick_fair), (void*) this);

    pthread_t idle_thread;
    pthread_create(&idle_thread, NULL, (&tick_idle), (void*) this);

    pthread_t pusher_thread;
    pthread_create(&pusher_thread, NULL, (&pusher), (void*) this);

    // wait for them
    pthread_join(fair_thread, NULL);
    pthread_join(idle_thread, NULL);
    pthread_join(pusher_thread, NULL);

}


void* CPU::tick_fair(void* arg)
{
    CPU *cpu = (CPU*) arg;
    printf("fair start\n");

    while(true)
    {
        usleep(time_delta);
        // TODO dormir si no hay tareas asignadas al cpu

        printf("tick_fair\n");
 		int goto_io = rand()%400; //(la probabilidad queda entre 0 y 0.25)

		if (goto_io <= cpu->running.io_prob)
		{
			cpu->idle_queue[rand()%WAITQUEUE_N].push(cpu->running);
            cpu->cfs_rq.lock();
			cpu->running = cpu->cfs_rq.get_min();
            cpu->cfs_rq.erase_min();
            cpu->cfs_rq.unlock();
  		}

 		cpu->running.v_runtime += time_delta;
        cpu->cfs_rq.lock();
        printf("%lld vs %lld\n",cpu->running.v_runtime, cpu->cfs_rq.get_min_v_runtime());
		if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime())
		{
			cpu->rbt_queue_push(cpu->running);
    		sem_post(&(cpu->rbt_queue_sem));
    		cpu->running = cpu->cfs_rq.get_min();
 		}
        cpu->cfs_rq.unlock();

    }
    return(NULL);
}

void* CPU::tick_idle(void* arg)
{
    printf("idle start\n");
    CPU *cpu = (CPU*) arg;
    while(true){
        usleep(time_delta);
        printf("tick_idle\n");

        for(int i=0; i<WAITQUEUE_N; i++){
            if(cpu->idle_queue[i].empty()) continue;
            int out_prob = rand()%200;
            if(out_prob <= cpu->idle_queue[i].idle_prob){
                TASK tsk = cpu->idle_queue[i].pop();
                cpu->rbt_queue_push(tsk);
            }
        }

    }
    return(NULL);
}

void* CPU::pusher(void* arg)
{
    CPU *cpu = (CPU*) arg;
    printf("pusher start\n");
    while(true){
        sem_wait(&cpu->rbt_queue_sem);

        printf("pusher pop push");

        TASK task = cpu->rbt_queue_pop();

        cpu->cfs_rq.lock();
        cpu->cfs_rq.insert(task);
        cpu->cfs_rq.unlock();

    }
    return(NULL);
}


void CPU::rbt_queue_push(TASK &task)
{
    pthread_mutex_lock(&rbt_queue_mutex);
    rbt_queue.push(task);
    pthread_mutex_unlock(&rbt_queue_mutex);
    sem_post(&rbt_queue_sem);
}

TASK CPU::rbt_queue_pop()
{
    pthread_mutex_lock(&rbt_queue_mutex);
    TASK task = rbt_queue.front();
    rbt_queue.pop();
    pthread_mutex_unlock(&rbt_queue_mutex);

    return task;
}

