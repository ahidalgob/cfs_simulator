#include <stdlib.h>
#include <time.h>

#include "task.h"

using namespace std;

TASK::TASK() {
	v_runtime = 0;
	nice = (rand() % 40) - 20;
	io_prob = rand() % 100;
	id=0;
	cpu=-1;
}
