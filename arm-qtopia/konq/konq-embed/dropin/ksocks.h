#ifndef __ksocks_h__
#define __ksocks_h__

#include <qglobal.h>

#if !defined(Q_WS_WIN)

#include <sys/types.h>
#include <netsupp.h>

#include <unistd.h>

class KSocks
{
public:
    inline int bind( int sockfd, sockaddr *my_addr, ksocklen_t addrlen )
    { return ::bind( sockfd, my_addr, addrlen ); }

    inline int listen( int s, int backlog )
    { return ::listen( s, backlog ); }

    inline int select( int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                       struct timeval *timeout)
    { return ::select( n, readfds, writefds, exceptfds, timeout ); }

    inline int accept( int s, sockaddr *addr, ksocklen_t *addrlen )
    { return ::accept( s, addr, addrlen ); }

    inline int connect( int sockfd, const sockaddr *serv_addr, ksocklen_t addrlen )
    { return ::connect( sockfd, serv_addr, addrlen ); }

    inline signed long int read ( int fd, void *buf, unsigned long int count )
    { return ::read( fd, buf, count ); }

    inline long int write( int fd, const void *buf, unsigned long int count )
    { return ::write( fd, buf, count ); }

    inline int getsockname( int s, sockaddr *name, ksocklen_t *namelen )
    { return ::getsockname( s, name, namelen ); }

    inline int getpeername( int s, sockaddr *name, ksocklen_t *namelen )
    { return ::getpeername( s, name, namelen ); }

    static KSocks *self()
    {
        if ( !s_self )
            s_self = new KSocks;
        return s_self;
    }

private:
    static KSocks *s_self;
};

#endif  

#endif

