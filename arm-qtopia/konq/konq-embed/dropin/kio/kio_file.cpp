/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include <config.h>

#include "kio_file.h"

#include <qdir.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>

using namespace KIO;

#define MAX_IPC_SIZE (1024*32)
#define HDR_IPC_SIZE (1024)

File::File( const QString &prot )
    : SlaveBase( prot.local8Bit() )
{
}

File::~File()
{
}

void File::get( const KURL &url )
{
    QString path = url.path();
#if defined(ENABLE_CGI)
    bool mayExec = false;
    bool asProxy = false;
    {
        KConfig *config = KGlobal::config();
        KConfigGroupSaver saver( config, QString::fromLatin1( "Local Protocols" ) );
	QString temp=config->readEntry( mProtocol, QString::null );
	if ( temp.left( 1 ) == "*" )
	{
	    temp.remove( 0, 1 );
	    mayExec = true;
	}
	else if ( temp.left( 1 ) == "!" )
	{
	    temp.remove( 0, 1 );
	    mayExec = true;
	    asProxy = true;
	    path="";
	}
	if ( temp.left( 1 ) == "~" )
	{
	    temp.remove( 0, 1 );
	    temp.prepend( QDir::homeDirPath() );
	}
	path=temp+path;
    }
    if ( path.find( "/../" ) != -1 || path.right( 3 ) == "/.." )
    {
        error( KIO::ERR_MALFORMED_URL, path );
        return;
    }
#endif

    QFileInfo info( path );

    if ( !info.exists() )
    {
        error( KIO::ERR_DOES_NOT_EXIST, path );
        return;
    }

    if ( info.isFile() )
    {
        QFile file( path );
	unsigned long bytes_read = 0;
	bool can_exec = info.isExecutable() &&
	     info.permission( QFileInfo::ExeUser | QFileInfo::ExeGroup | QFileInfo::ExeOther );

#if defined(ENABLE_CGI)
	QByteArray tryHeader;
	if ( mayExec && can_exec )
	{
	    QString referrer = metaData( "referrer" );
	    int pipefd[2];
	    if ( ::pipe( pipefd ) )
	    {
		error( KIO::ERR_UNKNOWN, path );
		return;
	    }

	    int pid = fork();
	    if ( pid == -1 )
	    {
		error( KIO::ERR_CANNOT_LAUNCH_PROCESS, path );
		return;
	    }

	    if ( pid == 0 )
	    { // in child here
		::close( pipefd [0] );
		if ( ::dup2( pipefd[1], 1 ) == -1 )
		    ::exit( errno );
		int i;
		for ( i=getdtablesize(); i >= 3 ; i-- )
		    ::close( i );
		::setenv( "GATEWAY_INTERFACE", "LOCAL-CGI/1.1", true );
		::setenv( "SCRIPT_NAME", url.path().latin1(), true );
		::setenv( "SCRIPT_FILENAME", path.latin1(), true );
		::setenv( "REQUEST_METHOD", "GET", true );
		::setenv( "REQUEST_URL", url.url().latin1(), true );
		::setenv( "REQUEST_URI", url.encodedPathAndQuery().latin1(), true );
		if ( url.hasHost() )
		    ::setenv( "SERVER_NAME", url.host().latin1(), true );
		if ( url.port() )
		    ::setenv( "SERVER_PORT", QString::number( url.port() ).latin1(), true );
		if ( !url.query().isEmpty() )
		    ::setenv( "QUERY_STRING", url.query().mid( 1 ).latin1(), true );
		if ( url.hasUser() )
		    ::setenv( "PROTOCOL_USER_NAME", url.user().latin1(), true );
		if ( url.hasPass() )
		    ::setenv( "PROTOCOL_USER_PASS", url.pass().latin1(), true );
		if ( !referrer.isEmpty() )
		    ::setenv( "PROTOCOL_REFERER", referrer.latin1(), true );
		::setenv( "PROTOCOL_NAME", mProtocol.data(), true );
		QDir::setCurrent( info.dirPath( true ) );
		execl( path.latin1(), path.latin1(), 0 );
		::exit( errno );
	    }
	    ::close( pipefd[1] );
	    if ( !file.open( IO_ReadOnly, pipefd[0] ) )
	    {
		::close( pipefd[0] );
        	error( KIO::ERR_CANNOT_OPEN_FOR_READING, path );
		return;
	    }
	    // Expect header lines, prepare to put back non-header data
	    tryHeader.resize( HDR_IPC_SIZE );
	    int n = 0;
	    bool eatNewline = false;
	    for ( ; ; )
	    {
		// QFile from pipe is very broken... we read each char
		//  we also can't rely on atEnd() and readch()
		QString h;
		for ( n = 0; n < HDR_IPC_SIZE; )
		{
		    char c;
		    if ( file.readBlock( &c, 1 ) != 1 )
			break;
		    tryHeader[n++] = c;
		    if ( c == '\n' )
			break;
		    if ( c != '\r' )
			h.append( c );
		}

		if ( h.isEmpty() )
		{
		    if ( eatNewline )
			n = 0;
		    break;
		}
		eatNewline = true;

		// HACK: KIO::ERR_COULD_NOT_LOGIN displays the message verbatim
		//       Use BELL ( \a, \007, ^G ) characters to insert newline
		if ( h.lower().startsWith( "error:" ) )
		    error( KIO::ERR_COULD_NOT_LOGIN, h.mid( 6 ).stripWhiteSpace()
					.replace( QRegExp( "\a" ), "\n" ) );
		else if ( h.lower().startsWith( "content-type:" ) )
		    mimeType( h.mid( 13 ).stripWhiteSpace() );
		else if ( h.lower().startsWith( "content-length:" ) )
		    totalSize( h.mid( 15 ).stripWhiteSpace().toUInt() );
		else if ( h.lower().startsWith( "location:" ) )
		    redirection( KURL( url, h.mid( 9 ).stripWhiteSpace() ) );
		else if ( h.lower().startsWith( "processed-size:" ) )
		    processedSize( h.mid( 15 ).stripWhiteSpace().toUInt() );
		else if ( h.lower().startsWith( "info-message:" ) )
		    infoMessage( h.mid( 13 ).stripWhiteSpace() );
		else if ( h.lower().startsWith( "meta-data:" ) )
		{
		    h = h.mid( 10 );
		    int i = h.find ( '=' );
		    if ( i > 0 )
			setMetaData( h.left( i ).stripWhiteSpace(),
				     h.mid( i+1 ).stripWhiteSpace() );
		}
		else
		    break; // will put back the n bytes read
	    }
	    tryHeader.resize( n );
	}

	else
#endif
	{
	    if ( !file.open( IO_ReadOnly ) )
	    {
        	error( KIO::ERR_CANNOT_OPEN_FOR_READING, path );
        	return;
	    }
	    totalSize( info.size() );
	    if ( can_exec )
		mimeType( "application/x-executable" );
        }


	for ( ; ; )
        {
            QByteArray buffer( MAX_IPC_SIZE );
	    int n = 0;

#if defined(ENABLE_CGI)
	    if ( !tryHeader.isEmpty() )
	    {
		// We have some data, put it back only once
		n = tryHeader.size();
		memcpy( buffer.data(), tryHeader.data(), n );
		tryHeader.resize( 0 );
	    }
#endif
	    n += file.readBlock( buffer.data()+n, buffer.size()-n );

	    if ( n == 0 )
		break;
            if ( n == -1 )
            {
                error( KIO::ERR_COULD_NOT_READ, path );
                return;
            }

            buffer.truncate( n );
	    bytes_read += n;

	    processedSize( bytes_read );

            data( buffer );
        }

        data( QByteArray() ); // signal end of data


    }
    else if ( info.isDir()
#if defined(ENABLE_CGI)
	      && !asProxy
#endif
			   )
    {
	int sort_mode = QDir::IgnoreCase | QDir::DirsFirst;
#ifdef SORT_DIRLIST
	QString q = url.query();
	if ( q.contains( "=D" ) )
	    sort_mode |= QDir::Reversed;
	if ( q.startsWith( "?U" ) )
	{
	    sort_mode |= QDir::Unsorted;
	    sort_mode &= ~QDir::DirsFirst;
	}
	else if ( q.startsWith( "?N=" ) )
	    sort_mode |= QDir::Name;
	else if ( q.startsWith( "?M=" ) )
	{
	    sort_mode |= QDir::Time;
	    // Qt ascending/descending sorting for time and size is buggy
	    sort_mode ^= QDir::Reversed;
	}
	else if ( q.startsWith( "?S=" ) )
	{
	    sort_mode |= QDir::Size;
	    sort_mode ^= QDir::Reversed;
	}
	else
#endif
	    sort_mode |= QDir::Name;
	mimeType( "inode/directory" );
        QDir dir( info.absFilePath(), QString::null, sort_mode );

        const QFileInfoList *entries = dir.entryInfoList();
	
	if ( entries )
	{
  	    totalSize( entries->count() );

	    // the header
	    QByteArray header_buffer;
	    QTextStream header_stream( header_buffer, IO_WriteOnly );
	    header_stream << QString::fromLatin1( "<html><body><pre>" );
#ifdef SORT_DIRLIST
	    header_stream << i18n( "<b>Sort:</b> "
		"<a href=\"?U\">Unsorted</a> "
		"<a href=\"?N=A\">+Name</a> "
		"<a href=\"?N=D\">-Name</a> "
		"<a href=\"?S=A\">+Size</a> "
		"<a href=\"?S=D\">-Size</a> "
		"<a href=\"?M=A\">+Date</a> "
		"<a href=\"?M=D\">-Date</a>" );
	    header_stream << QString::fromLatin1( "\n<hr>\n" );
#endif
	    data( header_buffer );

    	    QFileInfoListIterator it( *entries );

	    for ( unsigned long c = 0; it.current(); ++it )
	    {
		if ( it.current()->fileName() == "." ) continue;
		if ( it.current()->fileName() == ".." &&
		    url.path() == "/" ) continue;

		QString f_path( it.current()->absFilePath() );
		QString f_name( it.current()->fileName() );
		if ( it.current()->isDir() )
		{
		    f_name.prepend( "<b>" ).append( "</b>" );
		    if ( f_path.right( 1 ) != "/" && !it.current()->isSymLink() )
			f_path.append( "/" );
#ifdef SORT_DIRLIST
		    f_path.append( url.query() );
#endif
		}

	    	QByteArray buffer;

		QTextStream stream( buffer, IO_WriteOnly );

	        stream << QString::fromLatin1( "<a href=\"file:%1\">%2</a>\n" )
		    .arg( f_path )
		    .arg( f_name );

		processedSize( ++c );

		data( buffer );

	    }

	    // the footer
	    QByteArray footer_buffer;
	    QTextStream footer_stream( header_buffer, IO_WriteOnly );
	    footer_stream << QString::fromLatin1( "</pre></body></html>" );
	    data( footer_buffer );

	    data( QByteArray() );

        }
	
    }
    else
    {
        error( KIO::ERR_DOES_NOT_EXIST, path );
        return;
    }

    finished();
}

bool File::implementsProto( const QString &prot )
{
    if ( prot == "file" ) return true;
#if defined(ENABLE_CGI)
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "Local Protocols" ) );
    return !config->readEntry( prot, QString::null ).isEmpty();
#else
    return false;
#endif
}
