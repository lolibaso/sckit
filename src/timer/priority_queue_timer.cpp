#include "priority_queue_timer.h"
#include "time_util.h"

namespace sc {

priority_queue_timer::priority_queue_timer() 
{
    start_time_ = sc::time_util::now_usec(); 
    min_heap_.reserve(128); 
}

priority_queue_timer::~priority_queue_timer()
{
    clear();
}

void priority_queue_timer::clear()
{
    min_heap_.clear();
}


#define HEAP_ITEM_LESS(i, j) (min_heap_[(i)].expires < min_heap_[(j)].expires)

bool priority_queue_timer::siftdown(int x, int n)
{
    int i = x;
    for (;;)
    {
        int j1 = 2 * i + 1;
        if ((j1 >= n) || (j1 < 0)) // j1 < 0 after int overflow
        {
            break;
        }
        int j = j1; // left child
        int j2 = j1 + 1;
        if (j2 < n && !HEAP_ITEM_LESS(j1, j2))
        {
            j = j2; // right child
        }
        if (!HEAP_ITEM_LESS(j, i))
        {
            break;
        }
        std::swap(min_heap_[i], min_heap_[j]);
        min_heap_[i].index = i;
        min_heap_[j].index = j;
        i = j;
    }
    return i > x;
}

void priority_queue_timer::siftup(int j)
{
    for (;;)
    {
        int i = (j - 1) / 2; // parent node
        if (i == j || !HEAP_ITEM_LESS(j, i))
        {
            break;
        }
        std::swap(min_heap_[i], min_heap_[j]);
        min_heap_[i].index = i;
        min_heap_[j].index = j;
        j = i;
    }
}

TimerId 
priority_queue_timer::add_timer(uint32_t ms_time, TimerCallback cb)
{
    int64_t expire = sc::time_util::now_usec() - start_time_ + ms_time;
    TimerNode node;
    node.timer_id = next_counter();
    node.expires = expire;
    node.cb = cb;
    node.index = (int)min_heap_.size();
    min_heap_.push_back(node);
    siftup((int)min_heap_.size() - 1);

    return node.timer_id;
}

// This operation is O(n) complexity
bool priority_queue_timer::cancel_timer(TimerId id)
{
    for (int idx = 0; idx < (int)min_heap_.size(); idx++)
    {
        if (min_heap_[idx].timer_id == id)
        {
            int n = (int)min_heap_.size() - 1;
            int i = min_heap_[idx].index;
            if (i != n)
            {
                std::swap(min_heap_[i], min_heap_[n]);
                min_heap_[i].index = i;
                if (!siftdown(i, n))
                {
                    siftup(i);
                }
            }
            min_heap_.pop_back();
            return true;
        }
    }
    return false;
}

void priority_queue_timer::update()
{
    int64_t now = sc::time_util::now_usec() - start_time_;
    while (!min_heap_.empty())
    {
        TimerNode& node = min_heap_.front();
        if (now < node.expires)
        {
            break;
        }
        int n = (int)min_heap_.size() - 1;
        std::swap(min_heap_[0], min_heap_[n]);
        min_heap_[0].index = 0;
        siftdown(0, n);
        min_heap_.pop_back();
        if (node.cb)
        {
            node.cb();
        }
    }
}

}

