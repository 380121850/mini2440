#ifndef __kseparator_h__
#define __kseparator_h__

#include <qframe.h>

class KSeparator : public QFrame
{
public:
    inline KSeparator( int orientation, QWidget *parent, const char *name = 0 )
        : QFrame( parent, name )
    { setFrameStyle( orientation | QFrame::Sunken ); }
};

#endif
/*
 * vim:et
 */
