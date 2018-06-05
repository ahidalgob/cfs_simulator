#ifndef BALANCER_H
#define BALANCER_H

#include <queue>
#include <vector>
#include <pthread.h>

#include "balancer.h"
#include "task.h"
#include "CPU.h"
#include "waitqueue.h"

using namespace std;

class BALANCER
{
public:
	BALANCER(int, int, int);
	void push_to_idle(TASK&);
	vector <CPU*> cpu;

private:
	vector <WAITQUEUE*> wqs;
	static void* tick_idle(void*);
	int waitqueue_n;
	int time_delta;
};

#endif
