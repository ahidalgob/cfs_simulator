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