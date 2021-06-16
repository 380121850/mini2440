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

#include "krun.h"

#include "mimehandler.h"
#include "jobclasses.h"
#include "job.h"
#include "kprotocolmanager.h"
#include <kparts/browserextension.h>

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#if defined(_QT_QPE_)
#include <qpe/global.h>
#endif

// ###
#include <khtml_run.h>

KRun::KRun( const KURL &url, int, bool, bool )
{
    m_strURL = url;
    QTimer::singleShot( 0, this, SLOT( slotStart() ) );
    m_job = 0;
    m_fhandle = -1;
}

KRun::~KRun()
{
    if ( m_fhandle != -1 )
	::close( m_fhandle );
}

void KRun::runURL( const KURL& /*_url*/, const QString& /*_mimetype*/ )
{
}

void KRun::foundMimeType( const QString &/*type*/ )
{
}

void KRun::slotFoundMime( KIO::Job * /*job*/, const QString &/*type*/ )
{
// This is the place if we want really early decisions
}

void KRun::slotStart()
{
//    if ( !inherits( "KHTMLRun" ) )
//        foundMimeType( "text/html" );

    QString externalHandler = KProtocolManager::externalProtocolHandler( m_strURL.protocol() );
    if ( !externalHandler.isEmpty() )
    {
	if ( externalHandler.left( 1 ) == "@" )
	{
	    m_strURL = externalHandler.mid( 1 ) +
		       m_strURL.url().mid( m_strURL.url().find( ':' ) + 1 );
	}
	else
	{
	    if ( externalHandler.left( 1 ) != "!" )
    		exec( externalHandler, m_strURL.url() );

    	    emit error(); // stop the spinning wheel, err, the enabled stop button

    	    delete this; // pretend you don't see this :)
    	    return;
	}
    } 

    KIO::TransferJob *job = 0;

    // eeeeek!
    if ( inherits( "KHTMLRun" ) )
    {
	KHTMLRun *run = static_cast<KHTMLRun *>( this );

	if ( run->urlArgs().postData.size() > 0 )
	{
	    job = KIO::http_post( m_strURL, run->urlArgs().postData, false );
	    job->addMetaData( "content-type", run->urlArgs().contentType() );
	}
	else
	    job = KIO::get( m_strURL, false, false );

	job->addMetaData( run->urlArgs().metaData() );
    }
    else
	job = KIO::get( m_strURL, false, true );

    // ###
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
             this, SLOT( slotJobData( KIO::Job *, const QByteArray & ) ) );
    connect( job, SIGNAL( result( KIO::Job * ) ),
             this, SLOT( slotJobResult( KIO::Job * ) ) );
    connect( job, SIGNAL( mimetype( KIO::Job *, const QString & ) ),
             this, SLOT( slotFoundMime( KIO::Job *, const QString & ) ) );

    m_job = job;
}

void KRun::slotJobData( KIO::Job *job, const QByteArray &data )
{
    // Are we pumping data for an external program?
    if ( m_fhandle != -1 )
    {
	// This should *REALLY* be implemented in another thread or process
	if ( data.isEmpty() )
	{
	    emit error();
	    delete this;
	}
	else
	    for ( uint d = 0; d < data.count(); )
	    {
		int n = data.count() - d;
		if ( n > 1024 )
		    n = 1024;
		n = ::write( m_fhandle, data.data() + d, n );
		if ( n >= 0 )
		    d += n;
		else
		{
		    if ( errno == EAGAIN )
			usleep(5000);
		    else
		    {
			delete this;
			break;
		    }
		}
	    }
	return;
    }
    assert( job->inherits( "KIO::TransferJob" ) );
    assert( job == m_job );

    KIO::TransferJob *transferJob = static_cast<KIO::TransferJob *>( job );

    // take into account any redirections
    m_strURL = transferJob->url();

    QString referrer;
    {
	if ( transferJob->outgoingMetaData().contains( "referrer" ) )
	    referrer = transferJob->outgoingMetaData()["referrer"];
    }
    QString mtype = transferJob->mimetype();

    // if no MIME type is available attempt to guess it
    if ( mtype.isEmpty() )
    {
	const KIO::MimeHandler* guess = KIO::MimeHandler::Find( m_strURL );
	if ( !guess )
	    guess = KIO::MimeHandler::Find( data );

	if ( guess )
	    mtype = guess->Preferred();
	else
	    mtype = QString::fromLatin1( "text/html" );

    }

    // If app wants us to pump it data don't detach from job
    const KIO::MimeHandler* mime = KIO::MimeHandler::Find( mtype );
    if ( mime && mime->isExtApp() && mime->isExtApp()->wantsData() )
    {
	m_fhandle = exec( ( static_cast<const KIO::MimeExtApp *>( mime ) )->getExtApp(), m_strURL.url(), referrer, true );
	if ( m_fhandle >= 0 )
	{
	    // Since we already got some data call itself once
	    slotJobData( job, data );
	    return;
	}
    }

    transferJob->detach( data );

    job->disconnect( this, 0 );

    if ( mime && mime->isExtApp() )
    {
	job->kill();
	exec( mime->isExtApp()->getExtApp(), m_strURL.url(), referrer );
	emit error();
    }
    else if ( mime && mime->isUnknown() )
    {
	job->kill();
	emit error();
    }
    else
	foundMimeType( mtype );

    // check this again
    // this is extremely evil and relies on only one slot
    // connected to the signal. uhoh :)
    delete this;
}

void KRun::slotJobResult( KIO::Job *job )
{
    if ( job->error() )
    {
        job->showErrorDialog( 0 ); // ### parent
        emit error();
    }

    // this is extremely evil and relies on only one slot
    // connected to the signal. uhoh :)
    delete this;
}

int KRun::exec( const QString &app, const QString &arg, const QString &ref, bool passdata )
{
#if defined(_QT_QPE_)
    if ( !passdata )
    {
	Global::execute( app, arg ); 
	return -1;
    }
#endif
#if !defined(Q_WS_WIN)

    int filedes[2];
    filedes[1] = -1;

    if ( passdata && ::pipe( filedes ) )
	return -1;

    int pid = fork();
    if ( pid == -1 )
    {
	if ( passdata )
	{
	    ::close( filedes[0] );
	    ::close( filedes[1] );
	}
	return -1;
    }

    // parent
    if ( pid != 0 )
    {
	if ( passdata )
	{
	    ::close( filedes[0] );
	    ::fcntl( filedes[1], F_SETFL, O_NONBLOCK );
	}
	return filedes[1];
    }

    if ( passdata )
	::dup2( filedes[0], 0 );

    int i = getdtablesize();
    while ( i >= 3 )
    {
        ::close( i );
        i--;
    }

    if ( !ref.isEmpty() )
	::setenv( "PROTOCOL_REFERER", ref.latin1(), true );
    execlp( app.local8Bit().data(), app.local8Bit().data(), arg.local8Bit().data(), 0 );
    ::exit( errno );
#endif
    return -1;
}

#include "krun.moc"
