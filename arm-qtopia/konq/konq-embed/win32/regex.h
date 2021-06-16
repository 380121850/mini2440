#ifndef __regex_h__
#define __regex_h__

// ### REMOVEME: switch to pcre/win32
// for now: just make it compile

typedef int regex_t;

struct regmatch_t
{
    int rm_so;
    int rm_eo;
};

inline void regcomp( int *, const char *, int ) {}

inline void regfree( int * ) {}

inline bool regexec( int *, const char *, int, regmatch_t *, int ) { return false; }

#endif
