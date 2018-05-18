#include <queue>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "waitqueue.h"
#include "task.h"

using namespace std;

WAITQUEUE::WAITQUEUE() {
	srand (time(NULL));
	idle_prob = rand() % 100;
	pthread_mutex_init(&idle_mutex, NULL);
}

void WAITQUEUE::push(TASK &task){
	pthread_mutex_lock(&idle_mutex);
	idle_queue.push(task);
	pthread_mutex_unlock(&idle_mutex);
	return;
}
/*
TASK WAITQUEUE::front(){
	TASK task;
	pthread_mutex_lock(&idle_mutex);
	task = idle_queue.front();
	pthread_mutex_unlock(&idle_mutex);
	return task;
}*/

TASK WAITQUEUE::pop(){
	TASK task;
	pthread_mutex_lock(&idle_mutex);
	task = idle_queue.front();
	idle_queue.pop();
	pthread_mutex_unlock(&idle_mutex);
	return task;
}
