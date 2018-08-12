#include <errno.h>
#include <assert.h>
#include <vector>
#include <sys/epoll.h>
#include "event_demultiplexer.h"

namespace sc
{
namespace reactor
{
    epoll_demultiplexer::epoll_demultiplexer()
    {
        epoll_fd_ = ::epoll_create(FD_SETSIZE);
        assert(epoll_fd_ != -1);
        fd_num_ = 0;
    }

    epoll_demultiplexer::~epoll_demultiplexer()
    {
        ::close(epoll_fd_);
    }

    int epoll_demultiplexer::wait_events(
            std::map<handle_t, event_handler *> * handlers,
            int timeout, time_heap* event_timer)
    {
        std::vector<epoll_event> ep_evts(fd_num_);
        int num = epoll_wait(epoll_fd_, &ep_evts[0], ep_evts.size(), timeout);
        if (num > 0)
        {
            for (int idx = 0; idx < num; ++idx)
            {
                handle_t handle = ep_evts[idx].data.fd;
                assert(handlers->find(handle) != handlers->end());
                if ((ep_evts[idx].events & EPOLLERR) ||
                        (ep_evts[idx].events & EPOLLHUP))
                {
                    (*handlers)[handle]->handle_error();
                }
                else
                {
                    if (ep_evts[idx].events & EPOLLIN)
                    {
                        (*handlers)[handle]->handle_read();
                    }
                    if (ep_evts[idx].events & EPOLLOUT)
                    {
                        (*handlers)[handle]->handle_write();
                    }
                }
            }
        }
        if (event_timer != NULL)
        {
            event_timer->tick();
        }

        return num;
    }

    int epoll_demultiplexer::request_event(handle_t handle, event_t evt)
    {
        epoll_event ep_evt;
        ep_evt.data.fd = handle;
        ep_evt.events = 0;

        if (evt & kReadEvent)
        {
            ep_evt.events |= EPOLLIN;
        }
        if (evt & kWriteEvent)
        {
            ep_evt.events |= EPOLLOUT;
        }
        ep_evt.events |= EPOLLONESHOT;

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ep_evt) != 0)
        {
            if (errno == ENOENT)
            {
                if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ep_evt) != 0)
                {
                    return -errno;
                }
                ++fd_num_;
            }
        }
        return 0;
    }

    int epoll_demultiplexer::unrequest_event(handle_t handle)
    {
        epoll_event ep_evt;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handle, &ep_evt) != 0)
        {
            return -errno;
        }
        --fd_num_;
        return 0;
    }
} // namespace reactor
} // namespace sc

