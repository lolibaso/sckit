#ifndef _sc_reactor_event_demultiplexer_h_
#define _sc_reactor_event_demultiplexer_h_

#include <set>
#include <map>
#include "reactor.h"

namespace sc
{
namespace reactor
{
    class event_demultiplexer
    {
    public:

        virtual ~event_demultiplexer() {}

        virtual int wait_events(std::map<handle_t, event_handler *> * handlers,
                               int timeout = 0, time_heap* event_timer = NULL) = 0;
                               
        virtual int request_event(handle_t handle, event_t evt) = 0;

        virtual int unrequest_event(handle_t handle) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////

    class epoll_demultiplexer : public event_demultiplexer
    {
    public:
        epoll_demultiplexer();

        ~epoll_demultiplexer();

        virtual int wait_events(std::map<handle_t, event_handler *> * handlers,
                               int timeout = 0, time_heap* event_timer = NULL);

        virtual int request_event(handle_t handle, event_t evt);

        virtual int unrequest_event(handle_t handle);

    private:

        int  epoll_fd_;
        int  fd_num_;
    };
} // namespace reactor
} // namespace sc

#endif // _sc_reactor_event_demultiplexer_h_

