#ifndef __kstringhandler_h__
#define __kstringhandler_h__

#include <qstring.h>

class KStringHandler
{
public:
    static QString csqueeze( const QString &s, int ) { return s; }
};

#endif
/*
 * vim:et
 */
