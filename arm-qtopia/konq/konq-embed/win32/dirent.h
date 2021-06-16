#ifndef __dirent_h__
#define __dirent_h__

#include <qdir.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qcstring.h>

// this doesn't belong here, but kstddirs needs it aside from sys/stat.h .
// sys/stat.h is provided, except for the S_IS* macros.
// let's do it here :P

#define S_ISTYPE( mode, mask ) ((( mode ) & _S_IFMT ) & ( mask ) )

#define S_ISREG( mode ) S_ISTYPE( mode, S_IFREG )
#define S_ISDIR( mode ) S_ISTYPE( mode, S_IFDIR )

// same for realpath() ... not really the right place ...

// (grr, we have to make it take a QCString, because the compiler is
//  too stupid to see the const char * operator...)
char *realpath( const QCString &path, char *resolvedPath );

// oh, this the wrong place as well... :) anyway, this is just a dummy,
// it is never called

#ifndef GETHOSTNAMEDEFINED
inline int gethostname( char *, int ) { return -1; }
#endif

// now what we're really looking for

struct dirent
{
    const char *d_name; // valid as long as the DIR is valid
};

struct DIR
{
    DIR( const QStringList entries );
    inline ~DIR() { delete __entry; }

    dirent *__next();

    QStringList __entries;
    QStringList::ConstIterator __it;
    dirent *__entry;
};

inline DIR *opendir( const char * path )
{
    QDir d( QFile::decodeName( path ) );

    if ( !d.exists() || !d.isReadable() )
	return 0;

    return new DIR( d.entryList() );
}

inline dirent *readdir( DIR *d )
{
    return d->__next();
}

inline int closedir( DIR *d )
{
    delete d;
    return 0;
}

#endif
