#ifndef BALANCER_H
#define BALANCER_H

#include <queue>
#include <array>
#include <pthread.h>

#include "balancer.h"
#include "task.h"
#include "CPU.h"
#include "waitqueue.h"

#define WAITQUEUE_N 2
#define CPU_N 2

using namespace std;

class BALANCER
{
public:
	BALANCER();
	void push_to_idle(TASK&);
	CPU cpu[CPU_N];

private:
	WAITQUEUE wqs[WAITQUEUE_N];
	static void* tick_idle();
};

#endif
