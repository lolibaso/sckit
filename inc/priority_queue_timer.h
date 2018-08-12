#ifndef __sc_priority_queue_timer_h
#define __sc_priority_queue_timer_h
#include "timer_base.h"
#include <vector>
#include <unordered_map>

namespace sc {

class priority_queue_timer : public timer_base
{
public:
    struct TimerNode 
    {
        int     index = -1;
        TimerId timer_id = -1;
        int64_t expires = 0;
        TimerCallback cb;
    };

public:
    priority_queue_timer();
    virtual ~priority_queue_timer();

    TimerId  add_timer(uint32_t ms_time, TimerCallback cb) override;

    bool cancel_timer(TimerId id) override;

    void update() override; 

    int  size() const 
    {
        return min_heap_.size();
    }

private:
    void  clear();
    bool  siftdown(int x, int n);
    void  siftup(int j);

private:
    int64_t  start_time_;
    std::vector<TimerNode> min_heap_;
}; /* end of class priority_queue_timer */

}  /* end of namespace sc */


#endif /* end of __sc_priority_queue_timer_h */
