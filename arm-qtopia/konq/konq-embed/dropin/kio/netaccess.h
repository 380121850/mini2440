#ifndef __kio_netaccess_h__
#define __kio_netaccess_h__

#include <qfile.h>

#include <kurl.h>
#include <kfileitem.h>

namespace KIO
{

    class NetAccess
    {
    public:
        static bool stat( const KURL &src, KIO::UDSEntry &entry )
            { return ::stat( QFile::encodeName( src.path() ), &entry ) != -1; }

        static bool download( const KURL &src, QString &target );

        static void removeTempFile( const QString & );

        static QString lastErrorString();
    };

};

#endif
