
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
    bool ret = rb_tree.empty();
    return ret;
}
void CFS_RQ::insert(TASK tsk)
{
    rb_tree.insert(tsk);
}

void CFS_RQ::erase_min()
{
    if(!rb_tree.empty())
    {
        rb_tree.erase(rb_tree.begin());
    }
}

TASK CFS_RQ::get_min()
{
    TASK tsk;
    if(!rb_tree.empty())
    {
        tsk = *(rb_tree.begin());
    }
    return tsk;
}


long long CFS_RQ::get_min_v_runtime()
{
    long long ret=LLONG_MAX;
    if(!rb_tree.empty())
    {
        ret = rb_tree.begin()->v_runtime;
    }
    return ret;
}


