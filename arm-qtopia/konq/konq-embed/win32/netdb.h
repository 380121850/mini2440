#ifndef __netdb_h__
#define __netdb_h__

struct servent
{
    int s_port;
};

servent *getservbyname( const char *, const char * ) { return 0; }
int ntohs( int ) { return 0; }

#endif
