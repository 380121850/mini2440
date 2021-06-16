#ifndef __pwd_h__
#define __pwd_h__

// stub. not used. just to satisfy the compiler.
struct passwd
{
    const char *pw_dir;
};

// never called!
inline struct passwd *getpwnam( const char * ) { return 0; }

#endif

