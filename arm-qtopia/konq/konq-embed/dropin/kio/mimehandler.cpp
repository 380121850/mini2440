/*  This file is part of the KDE project
    Copyright (C) 2002 Paul Chitescu <paulc-devel@null.ro>

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

#include <kglobal.h>
#include <kconfig.h>

#include "mimehandler.h"

#include <qbuffer.h>
#include <qimage.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <kdebug.h>

using namespace KIO;

ExternalFilter* ExternalFilter::Create( const QString &app, const QString& type, const KURL &url )
{
    kdDebug() << "ExternalFilter::Create(\"" << app << "\",\"" 
	      << type << "\",\"" << url.url() << "\")" << endl;
    // HACK: archive handlers should create a query by prepending the
    //  internal archive filename with "??" (and possible other options)
    QString otype;
    if ( url.query().startsWith( "??" ) )
    {
	QString s = url.query().mid( 2 );
	if ( !s.startsWith( "/" ) )
	    s.prepend( '/' );
	const MimeHandler* mime = MimeHandler::Find( KURL( s ) );
	if ( mime )
	    otype = mime->isAlias() ? mime->isAlias()->outputType() : mime->Preferred();
    }

    int m_pipe_in[2], m_pipe_out[2];
    if ( ::pipe( m_pipe_out ) == 0 )
    {
	if ( ::pipe( m_pipe_in ) == 0 )
	{
	    int pid = fork();
	    if ( pid == -1 )
	    {
		::close( m_pipe_in[0] );
		::close( m_pipe_in[1] );
	    }
	    else if ( pid == 0 )
	    {
		int i;
		::dup2( m_pipe_in[1], 1 );
		::dup2( m_pipe_out[0], 0 );
		for ( i = ::getdtablesize(); i >= 3 ; i-- )
		    ::close( i );
		::setenv( "FILTER_MIME_IN", type.latin1(), TRUE );
		if ( !otype.isEmpty() )
		    ::setenv( "FILTER_MIME_OUT", otype.latin1(), TRUE );
		::setenv( "FILTER_URL", url.url().latin1(), TRUE );
		const char *exe = app.latin1();
		::execlp( exe, exe, 0 );
		::exit( errno );
	    }
	    else
	    {
		::close( m_pipe_in[1] );
		::close( m_pipe_out[0] );
		::fcntl( m_pipe_in[0], F_SETFL, O_NONBLOCK );
		::fcntl( m_pipe_out[1], F_SETFL, O_NONBLOCK );
		return new ExternalFilter( m_pipe_in[0], m_pipe_out[1], pid, otype );
	    }
	}
	::close( m_pipe_out[0] );
	::close( m_pipe_out[1] );
    }
    return 0;
}

ExternalFilter::~ExternalFilter()
{
    if ( m_file_in >= 0)
    {
	::close( m_file_in );
	m_file_in = -1;
    }
    if ( m_file_out >= 0)
    {
	::close( m_file_out );
	m_file_out = -1;
    }
}

void ExternalFilter::dataInput( const QByteArray &dat )
{
    // TODO: implement is asynchronously, would help A LOT
    uint i;
    if ( dat.isEmpty() )
    {
	closeOutput();
	return;
    }
    if ( m_file_out >= 0 )
    {
	for ( i = 0; i < dat.count(); )
	{
	    pumpData();
	    int n = dat.count() - i;
	    if ( n > 1024 )
		n = 1024;
	    n = ::write( m_file_out, dat.data() + i, n );
	    if ( n < 0 )
	    {
		if ( errno != EAGAIN )
		{
		    closeOutput();
		    break;
		}
	    }
	    else
		i += n;
	}
    }
    pumpData();
}

bool ExternalFilter::pumpData()
{
    if ( m_file_in < 0 )
	return false;

    for ( ;; )
    {
	QByteArray buff( 4096 );
	int n = ::read( m_file_in, buff.data(), buff.size() );
	if ( n < 0 )
	    return (errno == EAGAIN);
	else
	{
	    buff.truncate( n );
	    emit dataOutput( buff );
	    if ( n == 0 )
	    {
		::close( m_file_in );
		m_file_in = -1;
		break;
	    }
	}
    }
    return false;
}

void ExternalFilter::closeOutput()
{
    if ( m_file_out >= 0 )
    {
	::close( m_file_out );
	m_file_out = -1;
    }
    for ( ; pumpData(); );
}


QList<MimeHandler> MimeHandler::s_handlers;

const MimeHandler* MimeHandler::Find( const QString &type )
{
    const MimeHandler* retval = 0;
    int slash = type.find( '/' );
    for ( int i = 0; !retval && i < 3; i++ )
    {
	QString test_type = type.lower();
        switch ( i )
	{
	    case 1:
		if ( slash < 1 )
		    continue;
		test_type.replace( 0, slash, "*" );
		break;
	    case 2:
		if ( slash < 1 )
		    continue;
		test_type.replace( slash+1, 0xffff, "*" );
		break;
	    case 3:
		test_type = "*/*";
		break;
	}
	QListIterator<MimeHandler> it( s_handlers );
	int best = 0;
	int len;
	for ( ; it.current(); ++it )
	    if ( ( len = it.current()->Matches( test_type ) ) > best )
	    {
		best = len;
		retval = it.current();
	    }
    }
    return retval;
}

const MimeHandler* MimeHandler::Find( const KURL &url )
{
    const MimeHandler* retval = 0;
    QListIterator<MimeHandler> it( s_handlers );
    int best = 0;
    int len;
    for ( ; it.current(); ++it )
	if ( ( len = it.current()->Matches( url ) ) > best )
	{
	    best = len;
	    retval = it.current();
	}
    return retval;
}

const MimeHandler* MimeHandler::Find( const QByteArray &data )
{
    QListIterator<MimeHandler> it( s_handlers );
    for ( ; it.current(); ++it )
	if ( it.current()->Matches( data ) )
	    return it.current();
    return 0;
}

int MimeHandler::Matches( const QString &type ) const
{
    int best = 0;
    for ( QStringList::ConstIterator it = m_types.begin(); it != m_types.end(); it++ )
    {
	if ( type.startsWith( *it ) )
	{
	    int len = (*it).length();
	    QChar c = type.at( len );
	    if ( len > best && ( c == '\0' || c == ' ' || c == ';' ) )
		best = len;
	}
    }
    return best;
}

int MimeHandler::Matches( const KURL &url ) const
{
    int best = 0;
    QString ext = url.path().lower();
    int i = ext.findRev( '/' );
    if ( i >= 0 )
	ext.remove( 0, i+1 );
    i = ext.find( '.' );
    if ( i < 0 )
	return false;
    else if ( i > 0)
	ext.remove( 0, i );
    for ( QStringList::ConstIterator it = m_types.begin(); it != m_types.end(); it++ )
    {
	int len = (*it).length();
	if ( len > best && *it == ext.right( len ) )
	    best = len;
    }
    return best;
}

KParts::Factory* MimePart::getFactory()
{
    if ( !( m_factory || m_lib.isEmpty() ) )
    {
	QString name = m_lib;
	m_lib = QString::null;
	KLibrary *lib = KLibLoader::self()->globalLibrary( name.latin1() );
	if ( lib )
	{
	    KLibFactory *factory = lib->factory();
	    if ( factory && factory->inherits( "KParts::Factory" ) )
		m_factory = static_cast<KParts::Factory*>( factory );
	}
	if ( !m_factory )
	    kdWarning()<< "failed to load add-on '" << name << "'" <<endl;
    }
    return m_factory;
}

bool MimeInternalImage::Matches( const QByteArray &data ) const
{

    QBuffer buffer( data );
    buffer.open( IO_ReadOnly );
    return ( QImageIO::imageFormat( &buffer ) != 0 );
}

void MimeHandler::Initialize()
{
    // We should add internal handlers for built-in types
    // The first one should be the cannonical type
    AddHandler( new MimeInternalImage( "image/*" ) );
    QStringList tmp;
    tmp << "text/html" << ".html" << ".htm";
    tmp << "application/xhtml+xml" << "application/xhtml" << ".xhtml" << ".xht";
    AddHandler( new MimeInternal( tmp ) );
    tmp.clear();
    tmp << "text/xml" << "application/xml" << ".xml";
    AddHandler( new MimeInternal( tmp ) );
    tmp.clear();
    tmp << "text/javascript" << "application/x-javascript" << ".js";
    AddHandler( new MimeInternal( tmp ) );
    tmp.clear();
    tmp << "text/css" << ".css";
    AddHandler( new MimeInternal( tmp ) );

    typedef QMap<QString,QString> EntryMap;
    KConfig *config = KGlobal::config();
    EntryMap list=config->entryMap( "MIME Handlers" );
    EntryMap::Iterator item;
    for ( item = list.begin(); item != list.end(); item++ )
    {
	QStringList types = QStringList::split( ',', item.key().lower() );
	QString data = item.data();
	switch ( data.at( 0 ).latin1() )
	{
	    case '-':
		AddHandler( new MimeUnknown( types ) );
		break;
	    case '@':
		AddHandler( new MimeAlias( types, data.mid( 1 ) ) );
		break;
	    case '!':
		{
		    bool rw = false;
		    if ( "!" == data.mid( 1, 1 ) )
		    {
			rw = true;
			data.remove( 0, 1 );
		    }
		    AddHandler( new MimePart( types, data.mid( 1 ), rw ) );
		}
		break;
	    case '|':
		tmp = QStringList::split( '|', data, true );
		AddHandler( new MimeExtFilter( types, tmp[2], tmp[1] ) );
		break;
	    case '*':
		AddHandler( new MimeExtApp( types, data.mid( 1 ), true ) );
		break;
	    default:
		AddHandler( new MimeExtApp( types, data, false ) );
		break;
	}
    }
}

DataFilter* MimeExtFilter::createFilter ( const QString& type, const KURL &url ) const
{
    return ExternalFilter::Create( m_path, type.isEmpty() ? Preferred() : type, url );
}

#include "mimehandler.moc"
