#ifndef __sc_server_h
#define __sc_server_h

namespace sc {

typedef enum 
{
    SOCKET_UNUSED = 0,
    SOCKET_TCP_ACCEPT,
    SOCKET_TCP_LISTEN,
    SOCKET_TCP_CONNECTING,
    SOCKET_TCP_CONNECTED,
    SOCKET_TCP_RECONNECT_WAIT,
    SOCKET_UDP
} SCTXStat;

typedef struct 
{
    int                 sock;
    struct sockaddr_in  client_addr;
    time_t              create_time;
    time_t              last_access_time;
    SCTXStat            stat;
} SCTX;


class sc_server
{
public:
    sc_server(const char* ip, unsigned short port);
    ~sc_server();

public:
    void    loop();

private:
    char        ip_[64];
    uint16_t    port_;

};

}

#endif
