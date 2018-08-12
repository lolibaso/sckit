#ifndef __sc_timer_base_h
#define __sc_timer_base_h

#include <stdint.h>
#include <functional>
#include "noncopyable.h"

typedef std::function<void()>  TimerCallback;
typedef long  TimerId;

namespace sc {

class timer_base : protected noncopyable
{
public:
    timer_base() {}

    virtual ~timer_base() {};

    /* add  a timer to schedule after ms milliseconds */
    virtual TimerId  add_timer(uint32_t ms_time, TimerCallback cb) = 0;

    /* cancel the single timer named time_id */
    virtual bool cancel_timer(TimerId time_id) = 0;

    virtual void update() = 0;

    virtual int  size() const = 0;
    
protected:
    TimerId  next_counter() { return counter_++; }

    TimerId  counter_ = 0;
}; /* end of class timer_base */


} /* end of namespace sc */

#endif /* end of __sc_timer_base_h */

