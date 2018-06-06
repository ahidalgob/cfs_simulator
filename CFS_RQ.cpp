
#include "CFS_RQ.h"
#include "limits.h"

#include <algorithm>

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
	return rb_tree.empty();
}


int CFS_RQ::size()
{
	return rb_tree.size();
}

void CFS_RQ::insert(TASK tsk)
{
	rb_tree.insert(tsk);
}


TASK CFS_RQ::pop_min()
{
	TASK tsk;
	tsk.nice = 100;
	if(!rb_tree.empty())
	{
		tsk = *(rb_tree.begin());
		rb_tree.erase(rb_tree.begin());
	}
	return tsk;
}


long long int CFS_RQ::get_min_v_runtime()
{
	long long ret=LLONG_MAX;
	if(!rb_tree.empty())
	{
		ret = rb_tree.begin()->v_runtime;
	}
	return ret;
}

int CFS_RQ::get_id()
{
	int ret=0;
	if(!rb_tree.empty())
	{
		ret = rb_tree.begin()->id;
	}
	return ret;
}

int CFS_RQ::get_v_runtime()
{
	int ret=0;
	if(!rb_tree.empty())
	{
		ret = rb_tree.begin()->v_runtime;
	}
	return ret;
}

int CFS_RQ::get_2id()
{
	int ret=0;
	if(rb_tree.size()>1)
	{
		ret = (++rb_tree.begin())->id;
	}
	return ret;
}

int CFS_RQ::get_2v_runtime()
{
	int ret=0;
	if(rb_tree.size()>1)
	{
		ret = (++rb_tree.begin())->v_runtime;
	}
	return ret;
}

int CFS_RQ::get_3id()
{
	int ret=0;
	if(rb_tree.size()>2)
	{
		ret = (++(++rb_tree.begin()))->id;
	}
	return ret;
}

int CFS_RQ::get_3v_runtime()
{
	int ret=0;
	if(rb_tree.size()>2)
	{
		ret = (++(++rb_tree.begin()))->v_runtime;
	}
	return ret;
}
vector<TASK> CFS_RQ::rq_list()
{
    vector<TASK> list;
    for(multiset<TASK, task_compare>::iterator it=rb_tree.begin(); it!=rb_tree.end(); it++){
        list.push_back(*it);
    }
    reverse(list.begin(), list.end());
    return list;
}


