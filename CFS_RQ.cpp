
#include "CFS_RQ.h"
#include "limits.h"

CFS_RQ::CFS_RQ()
{
    pthread_mutex_init(&mutex, NULL);
}


void CFS_RQ::lock()
{
    pthread_mutex_lock(&mutex);
}

void CFS_RQ::unlock()
{
    pthread_mutex_unlock(&mutex);
}

bool CFS_RQ::empty()
{
    lock();
    bool ret = rb_tree.empty();
    unlock();
    return ret;
}
void CFS_RQ::insert(TASK tsk)
{
    lock();
    rb_tree.insert(tsk);
    unlock();
}

void CFS_RQ::erase_min()
{
    if(!rb_tree.empty())
    {
        lock();
        rb_tree.erase(rb_tree.begin());
        unlock();
    }
}

TASK CFS_RQ::get_min()
{
    TASK tsk;
    if(!rb_tree.empty())
    {
        lock();
        tsk = *(rb_tree.begin());
        unlock();
    }
    return tsk;
}


long long CFS_RQ::get_min_v_runtime()
{
    long long ret=LLONG_MAX;
    if(!rb_tree.empty())
    {
        lock();
        ret = rb_tree.begin()->v_runtime;
        unlock();
    }
    return ret;
}


