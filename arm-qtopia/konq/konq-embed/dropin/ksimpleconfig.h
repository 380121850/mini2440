#ifndef __ksimpleconfig_h__
#define __ksimpleconfig_h__

#include <kconfig.h>

// can't use typedef because it conflicts with kssld's forward decl :(
class KSimpleConfig : public KConfig
{
public:
    KSimpleConfig( const QString &fileName, bool readOnly = false )
        : KConfig( fileName, readOnly )
    {}
};

/*
 * vim: et sw=4
 */
#endif

