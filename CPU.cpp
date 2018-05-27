#include "CPU.h"
#include "waitqueue.h"

#include <unistd.h>

CPU::CPU()
{
    //rbtree.init() ?
    pthread_mutex_init(&rbt_mutex, NULL);

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
    while(true){
        usleep(500000);


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

        pthread_mutex_lock(&cpu->rbt_mutex);
        //cpu->rbtree.insert(task);
        pthread_mutex_unlock(&cpu->rbt_mutex);

    }
    return(NULL);
}


void CPU::rbt_queue_push(TASK &task)
{
    pthread_mutex_lock(&rbt_queue_mutex);
    rbt_queue.push(task);
    pthread_mutex_unlock(&rbt_queue_mutex);
}

// can be called only when rbt_queue.size()>0
TASK CPU::rbt_queue_pop()
{
    pthread_mutex_lock(&rbt_queue_mutex);
    TASK task = rbt_queue.front();
    rbt_queue.pop();
    pthread_mutex_unlock(&rbt_queue_mutex);

    return task;
}

