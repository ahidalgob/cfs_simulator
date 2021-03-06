#ifndef CFS_RQ_H
#define CFS_RQ_H

#include <pthread.h>
#include <set>
#include <vector>
#include "task.h"
using namespace std;

struct task_compare{
	bool operator() (const TASK& lhs, const TASK& rhs) const {
		return lhs.v_runtime < rhs.v_runtime;
	}
};

class CFS_RQ
{
public:
	CFS_RQ();
	//~CFS_RQ();
	void lock();
	void unlock();
	bool empty();
    int size();
	void insert(TASK);
	TASK pop_min();
	long long get_min_v_runtime();
	int get_id();
	int get_v_runtime();
	int get_2v_runtime();
	int get_2id();
	int get_3id();
	int get_3v_runtime();

    vector<TASK> rq_list();
private:
	pthread_mutex_t mutex;
	multiset<TASK, task_compare> rb_tree;
};



#endif
