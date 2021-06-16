/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "part.h"

#include <kio/job.h>
#include <klocale.h>
#include <kdebug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef TMP_MAX
#define TMP_MAX 32
#endif

using namespace KParts;

Part::Part( QObject *parent, const char *name )
    : QObject( parent, name )
{
    m_manager = 0;
    m_selectable = false;
    m_widget = 0;
    m_instance = 0;
}

Part::~Part()
{
    if ( m_widget )
    {
        // We need to disconnect first, to avoid calling it !
        disconnect( m_widget, SIGNAL( destroyed() ),
                    this, SLOT( slotWidgetDestroyed() ) );
        kdDebug(1000) << "deleting widget " << m_widget << " " << m_widget->name() << endl;
        delete (QWidget *)m_widget;
    }
}

Part *Part::hitTest( QWidget *widget, const QPoint & )
{
    if ( widget != m_widget )
        return 0;

    return this;
}

QWidget *Part::widget()
{
    return m_widget;
}

void Part::setWidget( QWidget *widget )
{
    ASSERT( !m_widget );
    m_widget = widget;
    connect( m_widget, SIGNAL( destroyed() ),
             this, SLOT( slotWidgetDestroyed() ) );
}

void Part::slotWidgetDestroyed()
{
    kdDebug() << "Part::slotWidgetDestroyed()" << endl;
    m_widget = 0;
    delete this;
}

bool Part::event( QEvent *event )
{
  if ( QObject::event( event ) )
    return true;

  if ( PartActivateEvent::test( event ) )
  {
    partActivateEvent( (PartActivateEvent *)event );
    return true;
  }

  if ( PartSelectEvent::test( event ) )
  {
    partSelectEvent( (PartSelectEvent *)event );
    return true;
  }

  if ( GUIActivateEvent::test( event ) )
  {
    guiActivateEvent( (GUIActivateEvent *)event );
    return true;
  }

  return false;
}

void Part::partActivateEvent( PartActivateEvent * )
{
}

void Part::partSelectEvent( PartSelectEvent * )
{
}

void Part::guiActivateEvent( GUIActivateEvent * )
{
}

ReadOnlyPart::ReadOnlyPart( QObject *parent, const char *name )
    : Part( parent, name ), m_file( "" ), m_job( 0 )
{
}

ReadOnlyPart::~ReadOnlyPart()
{
    closeURL();
}

bool ReadOnlyPart::openURL( const KURL &url )
{
    if ( url.isMalformed() || !closeURL() )
	return false;
    m_url = url;
    emit setWindowCaption( m_url.prettyURL() );
    if ( m_url.isLocalFile() && m_url.query().isEmpty() )
    {
	emit started( 0 );
	m_file = m_url.path();
	bool ret = openFile();
	if ( ret )
	    emit completed();
	return ret;
    }
    m_job = KIO::get( m_url, false, false );
    m_file = m_url.fileName();
    if ( m_url.isLocalFile() && m_url.query().startsWith( "??" ) )
    {
	m_file = m_url.query().mid( 2 );
	int sl = m_file.findRev( '/' );
	if ( sl >= 0 )
	    m_file.remove( 0, sl + 1 );
    }
    int extensionPos = m_file.findRev( '.' );
    if ( extensionPos > 0 )
	m_file.remove( 0, extensionPos );
    else
	m_file = "";
    // HACK: the m_job is assumed to be unique
    //  we also assume sizeof(m_job) == sizeof(ulong)
    m_file.prepend( QString::number( (ulong)m_job, 36 ) );
    m_file.prepend( QString::fromLatin1( "/tmp/.konqe-part-%1" ) );

    QCString fn;
    int fd = -1;
    for ( int i = 0; ( fd == -1 ) && ( i < TMP_MAX ); i++ )
    {
	fn = QFile::encodeName( m_file.arg( (ulong)::random(), 0, 36 ) );
	fd = ::open( fn.data(), O_RDWR | O_CREAT | O_EXCL, 0600 );
    }

    if ( fd == -1 )
    {
	abortLoad();
	emit canceled( i18n( "File I/O Error" ) );
	return false;
    }
    m_tempFile.setName( QFile::decodeName( fn ) );
    m_tempFile.open( IO_ReadWrite, fd );
    m_file = ""; // Don't allow accesses while downloading
    connect( m_job, SIGNAL( destroyed() ),
	     SLOT( slotJobDestroyed () ) );
    connect( m_job, SIGNAL( result( KIO::Job * ) ),
	     SLOT( slotJobFinished ( KIO::Job * ) ) );
    connect( m_job, SIGNAL( data( KIO::Job*, const QByteArray &) ),
	     SLOT( slotData( KIO::Job*, const QByteArray & ) ) );
    emit started( m_job );
    return true;
}

void ReadOnlyPart::abortLoad()
{
    m_file = "";
    if ( m_job )
    {
	KIO::Job *job = m_job;
	m_job = 0;
	job->kill();
    }
}

bool ReadOnlyPart::closeURL()
{
    abortLoad();
    if ( m_tempFile.exists() )
	m_tempFile.remove();
    m_tempFile.setName( "" );
    return true;
}

void ReadOnlyPart::slotJobDestroyed()
{
    if ( m_job )
    {
	kdWarning()<< "Unexpected kill of job " << m_job <<endl;
	m_job = 0;
	emit canceled( i18n( "Job killed" ) );
    }
}

void ReadOnlyPart::slotJobFinished( KIO::Job * job )
{
    ASSERT( job == m_job );
    m_job = 0;
    if ( job->error() )
	emit canceled( job->errorString() );
    else
    {
	m_file = m_tempFile.name();
	m_tempFile.close();
	openFile();
	emit completed();
    }
}

void ReadOnlyPart::slotData( KIO::Job * job, const QByteArray &data )
{
    ASSERT( job == m_job );
    m_tempFile.writeBlock( data );
}

void ReadOnlyPart::guiActivateEvent( GUIActivateEvent *event )
{
    if ( event->activated() )
    {
	if ( !m_url.isEmpty() )
	    emit setWindowCaption( m_url.prettyURL() );
	else
	    emit setWindowCaption( "" );
    }
}

#include "part.moc"

