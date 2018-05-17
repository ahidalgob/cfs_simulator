#ifndef CPU
#define CPU

#include <queue.h>
#include <vector.h>
#include <pthread.h>
#include <semaphore.h>

#include "task.h"
#include "rbtree.h"
#include "waitqueue.h"

using namespace std;

class CPU
{
public:
	CPU();
	~CPU();

private:
	RBTREE rbtree;
	TASK running;
	queue <TASK> rbt_queue;
	vector <WAITQUEUE> idle_queue;
	pthread_mutex_t rbt_mutex;
	sem_t rbt_queue_sem;

	// THREAD FUNCTIONS
	void* tick_fair(void*);
	void* tick_idle(void*);
	void* pusher(void*);

	// PUSHER
	void rbt_queue_push(&TASK);
	TASK rbt_queue_pop();
};


#endif
