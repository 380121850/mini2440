#ifndef __socket_h__
#define __socket_h__

// ### how to support TCP_NODELAY on windows?

#define TCP_NODELAY 42
#define IPPROTO_TCP 42

int setsockopt( int, int, int, void *, int ) { return -1; }

#endif
