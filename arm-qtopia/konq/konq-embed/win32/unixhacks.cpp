
#include <config.h>

#include <qcstring.h>

#include "unistd.h"
#include "dirent.h"

DIR::DIR( const QStringList entries )
    : __entries( entries ), __entry( 0 )
{
}

dirent *DIR::__next()
{
    if ( __entry == 0 )
    {
	    __it = __entries.begin();
	    __entry = new dirent;
    }
    else if ( __it != __entries.end() )
	    ++__it;

    if ( __it == __entries.end() )
	    return 0;

    __entry->d_name = (*__it).local8Bit();
    return __entry;
}

char *realpath( const QCString &path, char *resolvedPath )
{
    QCString rPath = QFile::encodeName( QDir::cleanDirPath( QFile::decodeName( path.data() ) ) );
    strncpy( resolvedPath, rPath.data(), MAXPATHLEN - 1 );
    resolvedPath[ MAXPATHLEN ] = 0;
    return resolvedPath;
}
