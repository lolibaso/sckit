#ifndef __sc_inet_addr_h
#define __sc_inet_addr_h

namespace sc 
{

class inet_addr 
{
public:
    inet_addr(void);
    ~inet_addr(void);

    void    set_port(uint16_t port);
    int     set_addr(const char* ip, uint16_t port);

    bool    is_loopback(void) const; 
    bool    is_multicast(void) const;

private:


};

}

#endif
