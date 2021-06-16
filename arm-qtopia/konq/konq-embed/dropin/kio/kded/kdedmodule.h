#ifndef __kdedmodule_h__
#define __kdedmodule_h__

#include <dcopobject.h>
#include <qobject.h>

class KDEDModule : public DCOPObject
{
    Q_OBJECT
public:
    KDEDModule( const QCString &name );
};

/*
 * vim: et sw=4 
 */
#endif

