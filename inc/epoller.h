#ifndef __sc_epoller_h
#define __sc_epoller_h
#include <sys/epoll.h>

namespace sc {

class epoller
{
    enum
    {
        EPOLL_READABLE  = 0x1,
        EPOLL_WRITEABLE = 0x2,
        EPOLL_ERRORABLE = 0x4,
    };

public:
    epoller(int max_fd = 1024);
    ~epoller();

public:
    int init(int max_fd);
    int add(int fd, int flag);
    int modify(int fd, int flag);
    int remove(int fd);
    int wait(int timeout_msec);

    int get_ready(int& pos, int& flag);

private:
    void set_ev(struct epoll_event& event, int fd, int flag);
    void get_ev(const struct epoll_event& event, int& fd, int& flag);

private:
    int max_fd_;
    int triggered_num_;
    int epoll_fd_;
    struct epoll_event* evs_;
}; /* end of class epoller */

}  /* end of namespace */

#endif /* end of __sc_epoller_h */

