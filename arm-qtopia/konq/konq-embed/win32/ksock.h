#ifndef __ksock_h__
#define __ksock_h__

// dummy stub to make kio/connection.cpp compile. this code is
// never called

typedef int ssize_t;

class KSocket
{
public:
    int socket() const { return -1; }
};

#endif
