#ifndef IDLE_H
#define IDLE_H

#include <queue>
#include <vector>
#include <pthread.h>

#include "task.h"
#include "waitqueue.h"

using namespace std;

class IDLEQUEUES
{
public:
	IDLEQUEUES();
	void push_to_idle(TASK&);

private:
	vector <WAITQUEUE> waitqueues;
	static void* tick_idle(void*);
	pthread_mutex_t idle_queue_mutex;
};

#endif
