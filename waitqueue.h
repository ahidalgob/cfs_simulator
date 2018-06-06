#ifndef WAITQUEUE_H
#define WAITQUEUE_H

#include <queue>
#include <pthread.h>
#include <set>
#include <vector>

#include "task.h"


using namespace std;

class WAITQUEUE
{
	queue <TASK> idle_queue;
	pthread_mutex_t idle_mutex;

public:
	WAITQUEUE();
	void push(TASK&);
	TASK pop();
	bool empty();
	int size();
	int idle_prob;
	int get_id();
	int get_last();
};

#endif
