#ifndef WAITQUEUE_H
#define WAITQUEUE_H

#include <queue>
#include <pthread.h>

#include "task.h"

using namespace std;

class WAITQUEUE
{
	queue <TASK> idle_queue;
	int idle_prob;
	pthread_mutex_t idle_mutex;
	
	public:
		WAITQUEUE();
		void push(TASK&);
		TASK pop();
		TASK front();
};

#endif
