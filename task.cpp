#include <stdlib.h>
#include <time.h>

#include "task.h"

//using namespace std;

TASK::TASK() {
	srand (time(NULL));
	v_runtime = 0;
	nice = (rand() % 40) - 20;
	io_prob = rand() % 100;
}
