#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "epoller.h"

namespace sc {

epoller::epoller(int max_fd) : max_fd_(max_fd), triggered_num_(0), epoll_fd_(-1) ,evs_(NULL)
{
}


epoller::~epoller()
{
    if(epoll_fd_ > 0) close(epoll_fd_);

    if(evs_) free(evs_);
}

int epoller::init(int max_fd)
{
    if(epoll_fd_ > 0) return 0;

    epoll_fd_ = epoll_create(max_fd);
    if(epoll_fd_ < 0)
    {
        return -1;
    }

    max_fd_ = max_fd;

    evs_ = (struct epoll_event*)malloc(sizeof(struct epoll_event) * max_fd_);
    if(NULL == evs_)
    {
        close(epoll_fd_);
        epoll_fd_ = -1;
        return -2;
    }
    memset(evs_, 0x0, sizeof(struct epoll_event) * max_fd_);

    return 0;
}

void epoller::set_ev(struct epoll_event& ev,  int fd, int flag)
{
    uint32_t events = 0;

    if(flag & EPOLL_READABLE)   events |= EPOLLIN;
    if(flag & EPOLL_WRITEABLE)  events |= EPOLLOUT;
    if(flag & EPOLL_ERRORABLE)  events |= EPOLLHUP | EPOLLERR;

    ev.data.fd = fd;
    ev.events  = events;
}

void epoller::get_ev(const struct epoll_event& ev, int& fd, int& flag)
{
    int f = 0;

    if(ev.events & EPOLLIN)      f |= EPOLL_READABLE;
    if(ev.events & EPOLLOUT)     f |= EPOLL_WRITEABLE;
    if(ev.events & (EPOLLHUP | EPOLLERR))      f |= EPOLL_ERRORABLE;

    fd = ev.data.fd;
    flag = f;
}

int epoller::add(int fd, int flag )
{
    struct epoll_event ev;

    if(fd < 0)
    {
        return -1;
    }

    set_ev(ev, fd, flag);

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        return errno; 
    }

    return 0;
}


int epoller::modify(int fd, int flag)
{
    struct epoll_event ev;

    if(fd < 0)
    {
        return -1;
    }

    set_ev(ev, fd, flag);

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0)
    {
        return errno;
    }

    return 0;
}


int epoller::remove(int fd)
{
    struct epoll_event ev;
    if(fd < 0)
    {
        return -1;
    }

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) < 0)
    {
        return errno;
    }

    return 0;
}

int epoller::wait(int timeout_msec)
{
    int num = -1;

    if(timeout_msec < 0)
    {
        timeout_msec = -1;  // wait indefinitely
    }

    num = epoll_wait(epoll_fd_, evs_, max_fd_, timeout_msec);
    if(num < 0)
    {
        if(errno != EAGAIN && errno != EINTR)
        {
            // LOG();
        }
        return -1;
    }

    triggered_num_ = num;

    return triggered_num_;
}

// 第一次使用之前，*pos应设置为0。返回非负数（fd）表示成功、负数表示出错或结束
int epoller::get_ready(int& pos, int& flag)
{
    int fd = -1 ;

    if(pos <0 )
    {
        return -1;
    }

	if( pos > triggered_num_ )
	{
		return -2;
	}

    get_ev((*(evs_ + pos*sizeof(struct epoll_event))), fd, flag);
    if(0 == flag)
    {
        return -3;
    }
    ++pos;

    return fd;
}

}  /* namespace of sc */


