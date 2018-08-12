#ifndef __sc_net_util_h
#define __sc_net_util_h
#include <string>
#include <vector>

namespace sc {

class net_util 
{
public:
    static bool is_big_endian();
    static bool is_little_endian();

    static int set_nonblocking(int fd);
    static int set_reuseaddr(int fd);
    static int set_keepalive(int fd);
    static int set_nodelay(int fd);
    static int set_broadcast(int fd);
    static int set_recvbuf(int fd, int size);
    static int set_sendbuf(int fd, int size);
    static void set_timeout(int fd, int ms);

    static char* addr2ip(struct sockaddr_in* addr, char* ip, int ip_len);

    static void  set_addr(const char* ip, unsigned short port, sockaddr_in * addr);

    static int create_udpsocket(const char* ip, unsigned short port);

    static int create_tcpsocket(const char* ip, unsigned short port);

    static int socket_getline(int sock, char* buf, int buf_len);

    static  int get_if_iplist(std::vector<std::string>& iplist);

    static char* get_local_ip(char* ip, int len);

}; // end of struct netutil

}  /* end of namespace sc */

#endif /* end of net_util.h */
