#ifndef CFS_RQ_H
#define CFS_RQ_H

#include <pthread.h>
#include <set>
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
    void insert(TASK);
    void erase_min();
    TASK get_min();
    long long get_min_v_runtime();
private:
    pthread_mutex_t mutex;
    set<TASK, task_compare> rb_tree;
};



#endif
