#ifndef CPU_H
#define CPU_H

#include <queue>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

#include "task.h"
#include "CFS_RQ.h"

using namespace std;

class CPU
{
public:
	CPU();

	// PUSHER
	void rbt_queue_push(TASK&);

	int number;
	bool busy;

private:
	CFS_RQ cfs_rq;

	TASK running;

	queue <TASK> rbt_queue;
	sem_t rbt_queue_sem;
	pthread_mutex_t rbt_queue_mutex;

	// THREAD FUNCTIONS
	static void* tick_fair(void*);
	static void* pusher(void*);

	// PUSHER
	TASK rbt_queue_pop();
	bool rbt_queue_empty();
};


#endif
