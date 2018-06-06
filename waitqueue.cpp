#include <queue>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "waitqueue.h"

using namespace std;

WAITQUEUE::WAITQUEUE() {
	idle_prob = rand() % 80 + 20;
	pthread_mutex_init(&idle_mutex, NULL);
}

void WAITQUEUE::push(TASK &task){
	pthread_mutex_lock(&idle_mutex);
	idle_queue.push(task);
	pthread_mutex_unlock(&idle_mutex);
	return;
}

int WAITQUEUE::size()
{
	return idle_queue.size();
}

TASK WAITQUEUE::pop(){
	TASK task;
	pthread_mutex_lock(&idle_mutex);
	task = idle_queue.front();
	idle_queue.pop();
	pthread_mutex_unlock(&idle_mutex);
	return task;
}

bool WAITQUEUE::empty()
{
	pthread_mutex_lock(&idle_mutex);
    bool ret = idle_queue.empty();
	pthread_mutex_unlock(&idle_mutex);
    return ret;
}


int WAITQUEUE::get_id()
{
	int ret=0;
	if(!idle_queue.empty())
	{
		ret = idle_queue.front().id;
	}
	return ret;
}

int WAITQUEUE::get_last()
{
	int ret = -1;
	if(!idle_queue.empty())
	{
		ret = idle_queue.front().last_cpu;
	}
	return ret;
}

