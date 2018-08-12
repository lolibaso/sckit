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
    typedef unsigned long timeid_t ;
    typedef int handle_t;
    typedef int (event_handler::*ev_io_callback)(handle_t);

    enum event_type_e
    {
        kReadEvent    = 0x01,
        kWriteEvent   = 0x02,
        kExceptEvent  = 0x04,
        kRWEEvents    = 0x07,
        kTimeoutEvent = 0x10,
        kSignalEvent  = 0x20,
        kEventMask    = 0xff
    };

    inline bool is_read_event(event_type_e e)
    {
        return (e & kReadEvent) == kReadEvent;
    }

    inline bool is_write_event(event_type_e e)
    {
        return (e & kWriteEvent) == kWriteEvent;
    }

    inline bool is_except_event(event_type_e e)
    {
        return (e & kExceptEvent) == kExceptEvent;
    }

    inline bool is_rwe_events(event_type_e e)
    {
        return is_read_event(e) && is_write_event(e) && is_except_event(e);
    }

    inline bool is_timeout_event(event_type_e e)
    {
        return (e & kTimeoutEvent) == kTimeoutEvent;
    }

    inline bool is_signal_event(event_type_e e)
    {
        return (e & kSignalEvent) == kSignalEvent;
    }

    inline bool is_all_events(event_type_e e)
    {
        return is_read_event(e)     && 
               is_write_event(e)    && 
               is_except_event(e)   &&
               is_signal_event(e)   &&
               is_timeout_event(e);
    }

    class event_handler
    {
    public:
        virtual handle_t get_handle() const = 0;
        virtual int handle_read(int fd)  { return -1; }
        virtual int handle_write(int fd) { return -1; }
        virtual int handle_except(int fd){ return -1; }
        virtual int handle_timeout(timeid_t tid) { return -1; }
        virtual int handle_signal(int signum){ return -1; }
        virtual int handle_close(int fd) { return -1; }

    protected:
        event_handler() {}
        virtual ~event_handler() { /* no-op */ }
    };

    class reactor_impl;

    class reactor : private noncopyable
    {
    public:
        reactor();
        ~reactor();

        int     register_handler(event_handler * handler, event_type_e evt);
        int     remove_handler(event_handler * handler);
        void    handle_events();
        int     register_timertask(heap_timer* timerevent);

    private:
        reactor_impl * reactor_impl_;
    };

} // namespace reactor
} // namespace sc
#endif // __sc_reactor_h

