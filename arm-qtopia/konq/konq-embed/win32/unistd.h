#ifndef __unistd_h__
#define __unistd_h__

#include <qglobal.h>
#include <qdir.h>

// include io.h for _pipe and _close
#include <io.h>

// ### include dirent so that kfileitem sees S_ISDIR
#include <dirent.h>

#include <qfileinfo.h>

// should be an enum but results in conflicts :-((
#define R_OK 1
#define W_OK 2
#define X_OK 4
#define F_OK 8

inline int access( const char *path, int mode )
{
    QFileInfo fi( QFile::decodeName( path ) );

    qDebug( "accessWrapper( %s )", path );

    if ( ( mode && R_OK ) && !fi.isReadable() )
        return -1;

    if ( ( mode && W_OK ) && !fi.isWritable() )
	return -1;

    if ( ( mode && X_OK ) && !fi.isExecutable() )
	return -1;

    if ( ( mode && F_OK ) && !fi.exists() )
	return -1;

    // ### correct?
    if ( mode == 0 )
	return 0;

    qDebug( "ACCESS OK!" );
    return 0;
}

// never called! just a dummy
inline int readlink( const char *, char *, int ) { return -1; }

// this one should be in sys/stat.h , but it isn't . wrap it here, for kstddirs

// ### what a hack :-P
typedef int mode_t;
typedef int pid_t;

inline int mkdir( const char *path, int /* ### mode ignored */ )
{
    QDir d;
    return d.mkdir( QFile::decodeName( path ) ) ? 0 : -1;
}

inline int fork()
{
    ASSERT( false );
    return -1;
}

inline int getpid() { return 0; } // not really important ;-)

#define SIGALRM 42
inline void alarm(int) {}
inline void signal( int, void (*) ) {} // really just a dummy for unused code in http.cc

// unused
#define S_IRUSR 0400
#define S_IRGRP 0040
#define S_IROTH 0004

#endif
