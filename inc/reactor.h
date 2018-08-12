#ifndef __sc_reactor_h
#define __sc_reactor_h

#include <stdint.h>
#include <unistd.h>
#include "noncopyable.h"
#include "heap_timer.h"

namespace sc
{
namespace reactor
{
    typedef unsigned int event_t;
    typedef int handle_t;

    enum
    {
        kReadEvent    = 0x01,
        kWriteEvent   = 0x02,
        kErrorEvent   = 0x04,
        kEventMask    = 0xff
    };

    class event_handler
    {
    public:
        virtual handle_t get_handle() const = 0;
        virtual void handle_read()  {}
        virtual void handle_write() {}
        virtual void handle_error() {}

    protected:
        event_handler() {}
        virtual ~event_handler() {}
    };

    class reactor_impl;

    class reactor : private noncopyable
    {
    public:
        reactor();
        ~reactor();

        int     register_handler(event_handler * handler, event_t evt);
        int     remove_handler(event_handler * handler);
        void    handle_events();
        int     register_timertask(heap_timer* timerevent);

    private:
        reactor_impl * reactor_impl_;
    };

} // namespace reactor
} // namespace sc
#endif // __sc_reactor_h

