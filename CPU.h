#ifndef CPU_H
#define CPU_H

#include <queue>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

#include "task.h"
#include "CFS_RQ.h"
#include "waitqueue.h"

using namespace std;

class CPU
{
public:
	CPU();
	//~CPU();

private:
	CFS_RQ cfs_rq;

    TASK running;
	vector <WAITQUEUE> idle_queue;

    queue <TASK> rbt_queue;
	sem_t rbt_queue_sem;
    pthread_mutex_t rbt_queue_mutex;

	// THREAD FUNCTIONS
	static void* tick_fair(void*);
	static void* tick_idle(void*);
	static void* pusher(void*);

	// PUSHER
	void rbt_queue_push(TASK&);
	TASK rbt_queue_pop();
};


#endif
