#ifndef __kwin_h__
#define __kwin_h__

#include <qwindowdefs.h>
#include <qwidget.h>

class NET
{
public:
    enum State { StaysOnTop };
};

class KWin
{
public:
    static void setState( WId, unsigned long ) {}
    static void setCurrentDesktop( int ) {}
    static void setOnDesktop( WId, int ) {}
    static int currentDesktop() { return 1; }

    struct Info
    {
        QRect geometry;
    };

    static Info info( WId id );
};

#endif
