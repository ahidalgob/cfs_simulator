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
	CPU(void*, long long int);

	// PUSHER
	void rbt_queue_push(TASK&);

    int size();
	int number;
	bool busy;
	pthread_mutex_t sync_mutex;

    vector<TASK> task_list();

private:
	CFS_RQ cfs_rq;

	TASK running;

	queue <TASK> rbt_queue;
	sem_t rbt_queue_sem;
	pthread_mutex_t rbt_queue_mutex;

	long long int time_delta;
	void* idleq;


	// THREAD FUNCTIONS
	static void* tick_fair(void*);
	static void* pusher(void*);

	// PUSHER
	TASK rbt_queue_pop();
	bool rbt_queue_empty();
	void rbt_queue_lock();
	void rbt_queue_unlock();
};


#endif
