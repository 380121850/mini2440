#ifndef __kstaticdeleter_h__
#define __kstaticdeleter_h__

// ### compat hack
#include <kglobal.h>

// hehe, Coolo would get grey hair when seeing this :-)

template <class T>
class KStaticDeleter
{
public:
    KStaticDeleter() {}

    T *setObject( T *obj ) { return obj; }
};

#endif
