#ifndef __sc_wheeltimer_h
#define __sc_wheeltimer_h

#include "timer_base.h"
#include <vector>
#include <unordered_map>

// timer queue implemented with hashed hierarchical wheel.
//
// inspired by linux kernel, see links below
// https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git/tree/kernel/timer.c?h=v3.2.98
//
// We model timers as the number of ticks until the next
// due event. We allow 32-bits of space to track this
// due interval, and break that into 4 regions of 8 bits.
// Each region indexes into a bucket of 256 lists.
//
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(1)       O(1)          O(1)
//

namespace sc {

enum
{
    WHEEL_BUCKETS = 4,
    TVN_BITS = 6,                   // time vector level shift bits
    TVR_BITS = 8,                   // timer vector shift bits
    TVN_SIZE = (1 << TVN_BITS),     // wheel slots of level vector
    TVR_SIZE = (1 << TVR_BITS),     // wheel slots of vector
    TVN_MASK = (TVN_SIZE - 1),      //
    TVR_MASK = (TVR_SIZE - 1),      //

    MAX_TVAL = ((uint64_t)((1ULL << (TVR_BITS + 4 * TVN_BITS)) - 1)),

//    TIME_UNIT = int64_t(1e7),       // centisecond, i.e. 1/100 second
    TIME_UNIT = int64_t(1e4),       // 
};

class wheel_timer : public timer_base
{
public:
    struct TimerNode
    {
        bool 	canceled = false;  // do lazy cancellation
        TimerId timer_id = -1;
        int64_t expires = 0;
        TimerCallback cb;
    };

    typedef std::vector<TimerNode*> TimerList;

public:
    wheel_timer();
    virtual ~wheel_timer();

    TimerId add_timer(uint32_t ms_time, TimerCallback cb) override;

    bool cancel_timer(TimerId id) override;

    void update() override;

    int  size() const override 
    { 
        return size_; 
    }

private:
    void tick();
    void add_timernode(TimerNode* node);
    bool cascade_timers(int bucket, int index);
    void clear_list(TimerList& list);
    void clear_all();
    TimerNode* alloc_node();
    void free_node(TimerNode* tn);

private:
    int 	size_ = 0;
    int64_t current_ = 0;
    int64_t jiffies_ = 0;
    TimerList near_[TVN_SIZE];
    TimerList buckets_[WHEEL_BUCKETS][TVR_SIZE];
    std::unordered_map<int, TimerNode*> ref_;       // make O(1) searching
    std::vector<TimerNode*>   free_list_;
};

}  /* end of namespace sc */

#endif 

