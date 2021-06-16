/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

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

// !! The history code is (c) David Faure <faure@kde.org> !!

#include "view.h"
#include "run.h"
#include "htmlview.h"
#include "browseriface.h"

#include <qtimer.h>
#include <qaction.h>
#include <qdir.h>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kio/job.h>
#include <kprotocolmanager.h>
#include <kstdaction.h>
#include <kconfig.h>
#include <klocale.h>

#include "defaults.h"

#include <assert.h>

#include <kdebug.h>

View::View( QObject *parent, const char *name, KParts::ReadOnlyPart *part )
    : QObject( parent, name ), m_part( part)
{
    m_zoom = DEFAULT_ZOOM;
    m_status = false;

    if ( m_part )
	setupActions();
}

View::~View()
{
    if ( m_part )
    {
	delete m_part;
	m_part = 0;
    }
}

void View::setupActions()
{

    connect( actionCollection(), SIGNAL( inserted(const char *) ),
             this, SIGNAL( actionChanged(const char *) ) );
    connect( actionCollection(), SIGNAL( removed(const char *) ),
             this, SIGNAL( actionChanged(const char *) ) );
    connect( m_part->actionCollection(), SIGNAL( inserted(const char *) ),
             this, SIGNAL( actionChanged(const char *) ) );
    connect( m_part->actionCollection(), SIGNAL( removed(const char *) ),
             this, SIGNAL( actionChanged(const char *) ) );

    connect( m_part, SIGNAL( setWindowCaption( const QString & ) ),
	     this, SLOT( setCaption( const QString & ) ) );
    connect( m_part, SIGNAL( setStatusBarText( const QString & ) ),
	     this, SLOT( statusMessage( const QString & ) ) );

    connect( m_part, SIGNAL( destroyed() ),
	     this, SLOT( partDestroyed() ) );

    if ( itemVisible( KStdAction::stdName( KStdAction::ShowStatusbar ) ) )
	KStdAction::showStatusbar( this, SLOT( toggleStatusBar(bool) ),
				   actionCollection() )->setChecked( m_status );
    View::toggleStatusBar( m_status );
}

void View::partDestroyed()
{
    m_part = 0;
    delete this;
}

void View::setItemVisible( const char *name, bool visible )
{
    QObject *obj = m_collection.child( name );
    if ( visible != ( obj != 0 ) )
    {
	if ( visible )
	    new QObject( actionCollection(), name );
	else
	    delete obj;
	emit actionChanged( name );
    }
}

void View::zoomTo( int zoomValue )
{
    m_zoom = zoomValue;
}

void View::setFindText( const QString &text )
{
    m_findText = text;
}

void View::toggleStatusBar( bool visible )
{
    m_status = visible;
    emit statusMessage( m_sbtext, m_status );
}

void View::setCaption( const QString &caption )
{
    m_caption = caption;
    emit captionChanged( m_caption );
}

void View::statusMessage( const QString &text )
{
    m_sbtext = text;
    emit statusMessage( m_sbtext, m_status );
}

void View::setLocationBarURL( const QString &url )
{
    m_locationBarURL = url;
    emit locationBarURLChanged( m_locationBarURL );
}

bool View::itemVisible( const char *name )
{
    return ( localVisible( name ) ||
	     ( m_part && m_part->actionCollection()->child( name ) ) );
}

void View::openURL( const KURL &url, const QString &/*mimeType*/, const KParts::URLArgs &_args )
{
    openURL( url, _args );
}

void View::openURL( const KURL &url, const KParts::URLArgs &args )
{
    if ( openNewURL( url, args ) )
	return;

    setLocationBarURL( url.prettyURL() );
    m_part->openURL( url );
}

bool View::openNewURL( const KURL &url, const KParts::URLArgs &args )
{
    QString externalHandler = KProtocolManager::externalProtocolHandler( url.protocol() );
    if ( externalHandler.left( 1 ) == "!" )
    {
	KParts::URLArgs newargs = args;
	int i = externalHandler.find( '!', 1 );
	if ( i > 0 )
	{
	    newargs.frameName = externalHandler.mid( 1, i-1 );
	    externalHandler.remove( 0, i+1 );
	}
	else
	    externalHandler.remove( 0, 1 );

	if ( newargs.frameName.isEmpty() )
	    newargs.frameName = QString::fromLatin1( "_blank" );
	else if ( newargs.frameName == "_" )
	    newargs.frameName.sprintf( "__%lx", ::random() );

	i = url.url().find( ':' );
        emit openURLRequest( this, KURL( externalHandler +
			     url.url().mid( i + 1 ) ),
			     newargs );
        return true;
    }
    return false;
}


// BrowserView global variables that are saved/restored from the config file
bool BrowserView::showLocation	= DEFAULT_LOCATIONBAR;
bool BrowserView::showStatusBar	= DEFAULT_STATUSBAR;
int  BrowserView::zoomFactor	= DEFAULT_ZOOM;

BrowserView::BrowserView( const QString &frameName, QWidget *parentWidget,
	    const char *widgetName, QObject *parent, const char *name, bool status )
    : View( parent, name ), m_frameName( frameName )
{
    m_history.setAutoDelete( true );

    m_stop = m_back = m_forward = 0;

    // don't specify a parent here as the javascript stuff can do
    // window.close() which results in a delete part; which in turn
    // we trigger in htmlViewDestroyed() and destroy us. Our QObject
    // destructor attempts to delete the child objects and whooops, we would
    // end up in double deletion of the part

    // pchitescu: we DO specify a parent - not us but the main window
    //  and we also (correctly) set the part name from the frame name
    m_doc = new HTMLView( parentWidget, widgetName, parent, m_frameName.latin1() );
    m_part = m_doc;
    m_status = status && isPlainBrowser();

    setupActions();

    connect( m_doc->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ),
             this, SLOT( openURL( const KURL &, const KParts::URLArgs & ) ) );

    connect( m_doc->browserExtension(), SIGNAL( openURLNotify() ),
             this, SLOT( openURLNotify() ) );

    connect( m_doc, SIGNAL( started( KIO::Job * ) ),
             this, SLOT( started( KIO::Job * ) ) );

    connect( m_doc, SIGNAL( completed() ),
             this, SLOT( completed() ) );

    connect( m_doc->browserExtension(), SIGNAL( setLocationBarURL( const QString & ) ),
             this, SLOT( setLocationBarURL( const QString & ) ) );

    m_browserIface = new BrowserInterface( this, "browseriface" );

    m_doc->browserExtension()->setBrowserInterface( m_browserIface );

    m_goBuffer = 0;
}

BrowserView::~BrowserView()
{
    if ( m_run )
    {
        if ( m_run->job() )
            m_run->job()->kill();

        delete static_cast<Run *>( m_run );
	m_run = 0;
    }
}

void BrowserView::setupActions()
{
    View::setupActions();

    if ( m_doc->isUtility() )
    {
	return;
    }

    // OK, this is a full web/file browser, add specific actions
    m_stop = new KAction( i18n( "Stop" ), 0,
			  this, SLOT( stop() ), actionCollection(), "go_stop" );
    m_back = KStdAction::back( this, SLOT( back() ), actionCollection() );
    m_forward = KStdAction::forward( this, SLOT( forward() ), actionCollection() );
    KStdAction::home( this, SLOT( home() ), actionCollection() );
    KStdAction::redisplay( this, SLOT( reload() ), actionCollection() );
    KStdAction::print( m_doc->browserExtension(), SLOT( print() ), actionCollection() );
    KStdAction::find( this, SLOT( find() ), actionCollection() );
    new KAction( i18n( "Close Find" ), 0,
		 this, SLOT( findClose() ), actionCollection(), "findClose" );
    new KAction( i18n( "Show Security Information" ), 0,
		 m_doc, SLOT( slotSecurity() ), actionCollection(), "info_security" );
    m_stop->setEnabled( false );
    m_back->setEnabled( false );
    m_forward->setEnabled( false );

    ( new KToggleAction( i18n( "Show Location Bar" ), 0,
		       this, SLOT( toggleLocationBar(bool) ), actionCollection(),
		       "options_show_location" ) )->setChecked( showLocation );
    toggleLocationBar( showLocation );

    if ( !itemVisible( KStdAction::stdName( KStdAction::ShowStatusbar ) ) )
	KStdAction::showStatusbar( this, SLOT( toggleStatusBar(bool) ),
				   actionCollection() )->setChecked( m_status );

    actionCollection()->clearNotifications();
    m_doc->actionCollection()->clearNotifications();

    View::zoomTo( zoomFactor );
    setScaling( m_doc );

    setItemVisible( "browser_bar" );
}

void BrowserView::setScaling( KHTMLPart *frame )
{
    if ( !frame || frame->scalingFactor() == m_zoom )
	return;

    frame->setScalingFactor( m_zoom );

    const QList<KParts::ReadOnlyPart> children = frame->frames();
    QListIterator<KParts::ReadOnlyPart> it( children );
    for (; it.current(); ++it )
	if ( it.current()->inherits( "KHTMLPart" ) )
	    setScaling( static_cast<KHTMLPart *>( it.current() ) );
}

void BrowserView::zoomTo( int zoomValue )
{
    zoomFactor = zoomValue;
    View::zoomTo( zoomValue );
    QApplication::setOverrideCursor( waitCursor );
    setScaling( m_doc );
    QApplication::restoreOverrideCursor();
}

void BrowserView::openURL( const KURL &url, const QString &mimeType, const KParts::URLArgs &_args )
{
    const KIO::MimeHandler* mime = KIO::MimeHandler::Find( mimeType );
    if ( mime && mime->isPart() )
    {
	KIO::MimePart *mpart = (KIO::MimePart*)mime;
	if ( m_run )
	{
	    setLocationBarURL( m_oldLocationBarURL );
	    if ( m_run->job() )
		m_run->job()->kill();
	}

	if ( m_stop ) m_stop->setEnabled( false );

	if ( m_history.count() > 0 )
	    updateHistoryEntry();

	KParts::Factory *factory = mpart->getFactory();
	if ( factory )
	    emit createPartView( url, factory, mpart->isReadWrite() );
	return;
    }

    createHistoryEntry();

    setLocationBarURL( url.prettyURL() );

    KParts::URLArgs args = _args;
    args.serviceType = mimeType;
    m_doc->browserExtension()->setURLArgs( args );

    m_doc->openURL( url );

    updateHistoryEntry();
}

void BrowserView::openURL( const KURL &url, const KParts::URLArgs &args )
{
    if ( openNewURL( url, args ) )
	return;

    if ( !args.frameName.isEmpty() )
    {
	emit openURLRequest( this, url, args );
	return;
    }
    if ( m_part->inherits( "KHTMLPart" ) && url.protocol() == "javascript" )
    {
	KHTMLPart *khtml = static_cast<KHTMLPart *>( m_part );
	if ( khtml->jScriptEnabled() )
	{
	    khtml->executeScript( url.prettyURL().replace( QRegExp( "javascript:/*" ), "" ) );
	    return;
	}
    }

    stop();

    m_run = new Run( this, url, args );
    connect( m_run, SIGNAL( error() ),
	     this, SLOT( runError() ) );

    m_oldLocationBarURL = m_locationBarURL;
    setLocationBarURL( url.prettyURL() );

    if ( m_stop ) m_stop->setEnabled( true );
}

void BrowserView::openURLNotify()
{
    updateHistoryEntry();
    createHistoryEntry();
}

void BrowserView::runError()
{
    setLocationBarURL( m_oldLocationBarURL );
    if ( m_stop ) m_stop->setEnabled( false );
}

void BrowserView::stop()
{
    if ( m_run )
    {
        setLocationBarURL( m_oldLocationBarURL );

        if ( m_run->job() )
            m_run->job()->kill();

        delete static_cast<Run *>( m_run );
    }

    m_doc->closeURL();

    if ( m_stop ) m_stop->setEnabled( false );

    if ( m_history.count() > 0 )
        updateHistoryEntry();
}

KURL BrowserView::homeURL()
{
    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "General" );
    return KURL( cfg->readEntry( "HomeURL", DEFAULT_HOMEPAGE ) );
}

void BrowserView::home()
{
    if ( locationBarURL().startsWith( "file:" ) )
	openURL( QDir::homeDirPath() );
    else
	openURL( homeURL() );
}

void BrowserView::back()
{
	stop();
	goHistory( -1 );
}

void BrowserView::forward()
{
	stop();
	goHistory( 1 );
}

void BrowserView::reload()
{
    KURL url = m_doc->url();
    if ( !url.isEmpty() && !url.isMalformed() )
    {
        KParts::URLArgs args;
        args.reload = true;
        m_doc->browserExtension()->setURLArgs( args );
        m_doc->openURL( url );
    }
}

void BrowserView::started( KIO::Job *job )
{
    if ( m_stop ) m_stop->setEnabled( true );

    if ( job )
        connect( job, SIGNAL( infoMessage( KIO::Job *, const QString & ) ),
                 this, SLOT( infoMessage( KIO::Job *, const QString & ) ) );
}

void BrowserView::completed()
{
    if ( m_stop ) m_stop->setEnabled( false );

    updateHistoryEntry();

    if ( m_back ) m_back->setEnabled( canGoBack() );
    if ( m_forward ) m_forward->setEnabled( canGoForward() );
}

void BrowserView::infoMessage( KIO::Job *, const QString &text )
{
    statusMessage( text );
}

void BrowserView::createHistoryEntry()
{
    HistoryEntry *current = m_history.current();

    if ( current )
    {
        m_history.at( m_history.count() - 1 );
        while ( m_history.current() != current )
            m_history.removeLast();
    }

    m_history.append( new HistoryEntry );
}

void BrowserView::updateHistoryEntry()
{
    HistoryEntry *current = m_history.current();

    assert( current );

    QDataStream stream( current->m_buffer, IO_WriteOnly );

    m_doc->browserExtension()->saveState( stream );

    current->m_url = m_doc->url();
}

void BrowserView::goHistoryDelayed( int steps )
{
    m_goBuffer = steps;
    QTimer::singleShot( 0, this, SLOT( goHistoryFromBuffer() ) );
}

void BrowserView::goHistoryFromBuffer()
{
    if ( m_goBuffer == 0 )
        return;

    int steps = m_goBuffer;
    m_goBuffer = 0;

    goHistory( steps );
}

void BrowserView::goHistory( int steps )
{
    int newPos = m_history.at() + steps;

    HistoryEntry *current = m_history.at( newPos );

    // I got sick of this de-sync between button status and history
    // Made it to ignore missing entries so it won't crash any more
    if ( !current )
	return;

    HistoryEntry h( *current );
    h.m_buffer.detach();

    QDataStream stream( h.m_buffer, IO_ReadOnly );

    m_doc->browserExtension()->restoreState( stream );

    if ( m_back ) m_back->setEnabled( canGoBack() );
    if ( m_forward ) m_forward->setEnabled( canGoForward() );
    setLocationBarURL( h.m_url.prettyURL() );
}

void BrowserView::findClose()
{
    setItemVisible( "search_bar", false );
}

void BrowserView::find()
{
    if ( localVisible( "search_bar" ) )
	findNext();
    else
    {
	setItemVisible( "search_bar" );
	m_doc->findTextBegin();
    }
}

void BrowserView::findNext()
{
    m_doc->findTextNext( findText(), true, false );
}

QStringList BrowserView::frameNames() const
{
    return childFrameNames( m_doc );
}

bool BrowserView::canHandleFrame( const QString &name, KParts::BrowserHostExtension **hostExtension )
{
    if ( !m_frameName.isEmpty() && m_frameName == name )
    {
        *hostExtension = 0;
        return true;
    }

    if ( !frameNames().contains( name ) )
        return false;

    *hostExtension = BrowserView::hostExtension( m_doc, name );
    return true;
}

QStringList BrowserView::childFrameNames( KParts::ReadOnlyPart *part )
{
  QStringList res;

  KParts::BrowserHostExtension *hostExtension = KParts::BrowserHostExtension::childObject( part );

  if ( !hostExtension )
    return res;

  res += hostExtension->frameNames();

  const QList<KParts::ReadOnlyPart> children = hostExtension->frames();
  QListIterator<KParts::ReadOnlyPart> it( children );
  for (; it.current(); ++it )
    res += childFrameNames( it.current() );

  return res;
}

KParts::BrowserHostExtension* BrowserView::hostExtension( KParts::ReadOnlyPart *part, const QString &name )
{
    KParts::BrowserHostExtension *ext = KParts::BrowserHostExtension::childObject( part );

  if ( !ext )
    return 0;

  if ( ext->frameNames().contains( name ) )
    return ext;

  const QList<KParts::ReadOnlyPart> children = ext->frames();
  QListIterator<KParts::ReadOnlyPart> it( children );
  for (; it.current(); ++it )
  {
    KParts::BrowserHostExtension *childHost = hostExtension( it.current(), name );
    if ( childHost )
      return childHost;
  }

  return 0;
}

void BrowserView::toggleLocationBar( bool visible )
{
    showLocation = visible;
    setItemVisible( "location_bar", visible );
}

void BrowserView::toggleStatusBar( bool visible )
{
    showStatusBar = visible;
    View::toggleStatusBar( visible );
}

bool BrowserView::isPlainBrowser() const
{
    return m_doc && !m_doc->isUtility();
}

#include "view.moc"
