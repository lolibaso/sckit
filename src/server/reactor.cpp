#include <assert.h>
#include "reactor.h"
#include "event_demultiplexer.h"

#define INITSIZE 100

namespace sc
{
namespace reactor
{
    class reactor_impl
    {
    public:
        reactor_impl();
        ~reactor_impl();

        int     register_handler(event_handler * handler, event_t evt);

        int     remove_handler(event_handler * handler);

        void    handle_events();

        int     register_timertask(heap_timer* timerevent);
    private:

        event_demultiplexer *                demultiplexer_;
        std::map<handle_t, event_handler *>  handlers_;
        time_heap* eventtimer_;
    };

    ///////////////////////////////////////////////////////////////////////////////

    reactor::reactor()
    {
        reactor_impl_ = new reactor_impl();
    }

    reactor::~reactor()
    {
        delete reactor_impl_;
    }

    int reactor::register_handler(event_handler * handler, event_t evt)
    {
        return reactor_impl_->register_handler(handler, evt);
    }

    int reactor::remove_handler(event_handler * handler)
    {
        return reactor_impl_->remove_handler(handler);
    }

    void reactor::handle_events()
    {
        reactor_impl_->handle_events();
    }

    int reactor::register_timertask(heap_timer* timerevent)
    {
        return reactor_impl_->register_timertask(timerevent);
    }

    ///////////////////////////////////////////////////////////////////////////////

    reactor_impl::reactor_impl()
    {
        demultiplexer_ = new epoll_demultiplexer();
        eventtimer_ = new time_heap(INITSIZE);
    }

    reactor_impl::~reactor_impl()
    {
        delete demultiplexer_;
    }

    int reactor_impl::register_handler(event_handler * handler, event_t evt)
    {
        handle_t handle = handler->get_handle();
        std::map<handle_t, event_handler *>::iterator it = handlers_.find(handle);
        if (it == handlers_.end())
        {
            handlers_[handle] = handler;
        }
        return demultiplexer_->request_event(handle, evt);
    }

    int reactor_impl::remove_handler(event_handler * handler)
    {
        handle_t handle = handler->get_handle();
        handlers_.erase(handle);
        return demultiplexer_->unrequest_event(handle);
    }

    //parm timeout is useless.
    void reactor_impl::handle_events()
    {
        int timeout = 0;
        if (eventtimer_->top() == NULL)
        {
            timeout = 0;
        }
        else
        {
            timeout = ((eventtimer_->top())->expire - time(NULL)) * 1000;
        }
        demultiplexer_->wait_events(&handlers_, timeout, eventtimer_);
    }

    int reactor_impl::register_timertask(heap_timer* timerevent)
    {
        if (timerevent == NULL)
            return -1;
        eventtimer_->add_timer(timerevent);
        return 0;
    }

} // namespace reactor
} // namespace sc

