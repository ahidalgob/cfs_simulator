#include "CPU.h"
#include "waitqueue.h"
#include <stdlib.h>

#include <unistd.h>

#define time_delta 5000000

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

    while(true)
    {
        usleep(time_delta);
 		int goto_io = rand()%100;

		if (goto_io <= cpu->running.io_prob)
		{
			cpu->idle_queue[rand()%WAITQUEUE_N].push(cpu->running);
            cpu->cfs_rq.lock();
			cpu->running = cpu->cfs_rq.get_min();
            cpu->cfs_rq.remove_min();
            cpu->cfs_rq.unlock();

  		}

 		cpu->running.v_runtime += time_delta;
        cpu->cfs_rq.lock();
		if (cpu->running.v_runtime > cpu->cfs_rq.get_min_v_runtime())
		{
			cpu->rbt_queue_push(cpu->running);
    		sem_post(&(cpu->rbt_queue_sem));
    		cpu->running = cpu->cfs_rq->rb_get_min();
 		}
        cpu->cfs_rq.unlock();

    }
    return(NULL);
}

void* CPU::tick_idle(void* arg)
{
    CPU *cpu = (CPU*) arg;
    while(true){
        usleep(500000);


    }
    return(NULL);
}

void* CPU::pusher(void* arg)
{
    CPU *cpu = (CPU*) arg;
    while(true){
        sem_wait(&cpu->rbt_queue_sem);

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

