#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ifaddrs.h>
#include "net_util.h"

namespace sc {


bool net_util::is_big_endian()
{
    const int n = 1;

    return ((char*)&n)[0] == 0;
}

bool net_util::is_little_endian()
{
    return !is_big_endian();
}


int net_util::set_nonblocking(int fd)
{
    int flag ;

    flag = fcntl(fd, F_GETFL, 0);
    if(flag < 0)
    {
        return -1;
    }

    if(flag & O_NONBLOCK)
    {
        return 0;
    }

    if(0 > fcntl(fd, F_SETFL, flag|O_NONBLOCK)) 
    {
        return -2;
    }

    return 0;
}

int net_util::set_reuseaddr(int fd)
{
    int flag = 1;

    if(0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) )
    {
        return -1;
    }

    return 0;
}

int net_util::set_nodelay(int fd)
{
    int flag = 1;

    if(0 > setsockopt(fd, IPPROTO_TCP, TCP_NODELAY , &flag, sizeof(flag)) )
    {
        return -1;
    }

    return 0;
}

int net_util::set_broadcast(int fd)
{
    int flag = 1;

    if(0 > setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) )
    {
        return -1;
    }

    return 0;
}

int net_util::set_keepalive(int fd)
{
    int flag = 1;

    if(0 > setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) )
    {
        return -1;
    }

    return 0;
}

int net_util::set_recvbuf(int fd, int size)
{

    if(0 > setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size)) )
    {
        return -1;
    }

    return 0;
}

int net_util::set_sendbuf(int fd, int size)
{

    if(0 > setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size)) )
    {
        return -1;
    }

    return 0;
}

void net_util::set_timeout(int fd, int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}


void net_util::set_addr(const char* ip, unsigned short port, sockaddr_in * addr)
{
    memset(addr, 0x0, sizeof(*addr));

    addr->sin_family = AF_INET;
#ifdef INET6
    inet_pton(AF_INET6, ip, &(addr->sin_addr));
#else
    inet_pton(AF_INET, ip, &(addr->sin_addr));
#endif
    addr->sin_port   = htons(port);

    return;
}

char* net_util::addr2ip(struct sockaddr_in* addr, char* ip, int ip_len)
{
#ifdef INET6
    inet_ntop(AF_INET, &(addr->sin_addr), ip, ip_len);
#else
    inet_ntop(AF_INET6, &(addr->sin_addr), ip, ip_len);
#endif

    return ip;
}

int net_util::create_udpsocket(const char* ip, unsigned short port)
{
    struct sockaddr_in addr;
    int s = 0;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == -1)
	{
        return -1;
	}
	
    if( set_nonblocking(s) < 0 || set_reuseaddr(s) < 0 )
    {
        close(s);
        return -2;
    }

    set_addr(ip, port, &addr);

    if(bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        close(s);
        return -3;
    }

    return s;
}


int net_util::create_tcpsocket(const char* ip, unsigned short port)
{
    struct sockaddr_in addr;
    int s = 0;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1)
    {
        return -1;
    }
    if( set_nonblocking(s) < 0 ) 
    {
        close(s);
        return -2;
    }

    if(set_reuseaddr(s) <0)
    {
        close(s);
        return -3;
    }

    set_addr(ip, port, &addr);
    
    if(bind(s, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        close(s);
        return -4;
    }
    if(listen(s, 1024) == -1)
    {
        close(s);
        return -5;
    }
    return s;
}

int net_util::socket_getline(int sock, char* buf, int buf_len)
{
	int i = 0;
	char c = '\0';
	int n;

	while ((i < buf_len - 1) && (c != '\n'))
	{
	 n = recv(sock, &c, 1, 0);
	 if (n > 0)
	 {
		 if (c == '\r')
		 {
			 n = recv(sock, &c, 1, MSG_PEEK);
			 if ((n > 0) && (c == '\n'))
				 recv(sock, &c, 1, 0);
			 else
				 c = '\n';
		 }
		 buf[i] = c;
		 i++;
	 }
	 else
		 c = '\n';
	}
	buf[i] = '\0';

	return(i);
}

int net_util::get_if_iplist(std::vector<std::string>& ip_list)
{
	struct ifaddrs *addrs;
	void *	tmp_ptr = NULL;
	char 	ip[INET_ADDRSTRLEN];

	getifaddrs(&addrs);
	while (addrs != NULL) 
	{
		if (addrs->ifa_addr->sa_family==AF_INET) 
		{
			tmp_ptr=&((struct sockaddr_in *)addrs->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp_ptr, ip, INET_ADDRSTRLEN);
			//printf("%s IP Address:%s\n", addrs->ifa_name, ip);

			ip_list.push_back(std::string(ip));
		}
		addrs=addrs->ifa_next;
	}
	//free ifaddrs
	freeifaddrs(addrs);

	return ip_list.size();
}

char* net_util::get_local_ip(char* ip, int len)
{
    std::vector<std::string> ip_list;

    get_if_iplist(ip_list);

    for(std::vector<std::string>::iterator it=ip_list.begin(); it!=ip_list.end(); it++) 
    {
        if( 0 == it->compare(0, 3, "192") ||
            0 == it->compare(0, 3, "192") ||
            0 == it->compare(0, 3, "10.") )
            strncpy(ip, it->c_str(), len);  
    }
    
    return ip;
}

}
#if 0
#include <stdio.h>
int main()
{
    std::vector<std::string> iplist;

    int n = sc::net_util::get_if_iplist(iplist);

    for(int i=0; i<n; i++)
    {
        printf("%s\n", iplist[i].c_str());
    }
   
    char ip[20];
    printf("localip:%s\n", sc::net_util::get_local_ip(ip, 20));
    return 0;
}
#endif
