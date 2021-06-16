
#include "kmimemagic.h"

KMimeMagic *KMimeMagic::s_self = 0;

KMimeMagic *KMimeMagic::self()
{
    if ( !s_self )
        s_self = new KMimeMagic;
    return s_self;
}

