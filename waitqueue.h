#ifndef WAITQUEUE
#define WAITQUEUE

#include <queue.h>
#include <pthread.h>

#include "task.h"

using namespace std;

class WAITQUEUE
{
public:
	WAITQUEUE();
	~WAITQUEUE();
	void push(&TASK);
	TASK pop();

private:
	queue <TASK> ;
	int prob;
	pthread_mutex_t idle_mutex;
};


#endif
