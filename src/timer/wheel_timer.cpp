// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "wheel_timer.h"
#include "../time_util.h"
#include <assert.h>
#include <chrono>

namespace sc {

wheel_timer::wheel_timer()
{
	current_ =  sc::time_util::now_usec();
//    current_ = std::chrono::system_clock::now();
    ref_.rehash(128);            // reserve a little space
    free_list_.reserve(1024);    // max free list item count
}


wheel_timer::~wheel_timer()
{
    clear_all();
}

void wheel_timer::clear_list(TimerList& list)
{
    for (auto ptr : list)
    {
        delete ptr;
    }
    list.clear();
}

void wheel_timer::clear_all()
{
    for (int i = 0; i < (int)TVN_SIZE; i++)
    {
        clear_list(near_[i]);
    }
    for (int i = 0; i < (int)WHEEL_BUCKETS; i++)
    {
        for (int j = 0; j < (int)TVR_SIZE; j++)
        {
            clear_list(buckets_[i][j]);
        }
    }
    ref_.clear();
}

wheel_timer::TimerNode* wheel_timer::alloc_node()
{
    TimerNode* node = nullptr;
    if (free_list_.size() > 0)
    {
        node = free_list_.back();
    }
    else
    {
        node = new TimerNode;
    }
    return node;
}

void wheel_timer::free_node(TimerNode* node)
{
    if (free_list_.size() < free_list_.capacity())
    {
        free_list_.push_back(node);
    } 
    else 
    {
        delete node;
    }
}

void wheel_timer::add_timernode(TimerNode* node)
{
    int64_t expires = node->expires;
    uint64_t idx = (uint64_t)(expires - jiffies_);
    TimerList* list = nullptr;
    if (idx < TVR_SIZE) // [0, 0x100)
    {     
        int i = expires & TVR_MASK;
        list = &near_[i];
    }
    else if (idx < (1 << (TVR_BITS + TVN_BITS))) // [0x100, 0x4000)
    {
        int i = (expires >> TVR_BITS) & TVN_MASK;
        list = &buckets_[0][i];
    }
    else if (idx < (1 << (TVR_BITS + 2 * TVN_BITS))) // [0x4000, 0x100000)
    {
        int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
        list = &buckets_[1][i];
    }
    else if (idx < (1 << (TVR_BITS + 3 * TVN_BITS))) // [0x100000, 0x4000000)
    {
        int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[2][i];
    }
    else if ((int64_t)idx < 0)
    {
        // Can happen if you add a timer with expires == jiffies,
        // or you set a timer to go off in the past
        int i = jiffies_ & TVR_MASK;
        list = &near_[i];
    }
    else
    {
        // If the timeout is larger than MAX_TVAL on 64-bit
        // architectures then we use the maximum timeout
        if (idx > MAX_TVAL)
        {
            idx = MAX_TVAL;
            expires = idx + jiffies_;
        }
        int i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[3][i];
    }
    // add to linked list
    list->push_back(node);
}

TimerId 
wheel_timer::add_timer(uint32_t ms_time, TimerCallback cb)
{
    TimerNode* node = alloc_node();
    node->canceled = false;
    node->cb = cb;
    node->expires = jiffies_ + ms_time;
    node->timer_id = next_counter();
    add_timernode(node);
    ref_[node->timer_id] = node;
    size_++;
    return node->timer_id;
}

// Do lazy cancellation, so we can effectively use vector as container of timer nodes
bool wheel_timer::cancel_timer(TimerId id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        node->canceled = true;
        size_--;
        return true;
    }
    return false;
}

// cascade all the timers at bucket of index up one level
bool wheel_timer::cascade_timers(int bucket, int index)
{
    // swap list
    TimerList list;
    buckets_[bucket][index].swap(list);

    for (auto node : list)
    {
        add_timernode(node);
    }
    return index == 0;
}

#define INDEX_POS(N) ((jiffies_ >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

// cascades all vectors and executes all expired timer
void wheel_timer::tick()
{
    int index = jiffies_ & TVR_MASK;
    if (index == 0) // cascade timers
    {
        if (cascade_timers(0, INDEX_POS(0)) && 
            cascade_timers(1, INDEX_POS(1)) && 
            cascade_timers(2, INDEX_POS(2)))
            cascade_timers(3, INDEX_POS(3));
    }
#undef INDEX_POS
    
    ++jiffies_;

    // swap list
    TimerList expired;
    near_[index].swap(expired);

    for (auto node : expired)
    {
        if (!node->canceled && node->cb)
        {
            node->cb();
            size_--;
        }
        
        ref_.erase(node->timer_id);
        delete node;
    }
}

void wheel_timer::update()
{
    int64_t now =sc::time_util::now_usec();
//    int 64_t now = std::chrono::system_clock::now(); 
    if (now < current_)
    {
       // LOG(ERROR) << "time go backwards: " << now << ", " << current_;
        return;
    }
    else if (now >= current_)
    {
        int64_t ticks = (now - current_) / TIME_UNIT;
        if (ticks > 0)
        {
            current_ = now;
            for (int i = 0; i < ticks; i++)
            {
                tick();
            }
        }
    }
}

}
