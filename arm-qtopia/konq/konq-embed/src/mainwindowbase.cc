/*  This file is part of the KDE project
    Copyright (C) 2000,2001 Simon Hausmann <hausmann@kde.org>

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

#include <config.h>

#include <klocale.h>
#include <kstddirs.h>
#include <kstdaction.h>

#include <qpopupmenu.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qabstractlayout.h>
#include <qrect.h>
#include <qfile.h>
#include <qobjectlist.h>
#include <qasciidict.h>

#ifndef QT_NO_CLIPBOARD
#include <qclipboard.h>
#endif

#ifdef _QT_QPE_
#include <qpe/global.h>
#endif

#include <kconfig.h>
#include <kmessagebox.h>
#include <kcharsets.h>
#include <dcopclient.h>

#include <assert.h>

#include <kparts/factory.h>
#include <khtml_ext.h>
#include <khtml_factory.h>
#include <khtml_settings.h>
#include <kio/mimehandler.h>

#include <misc/htmltags.h>
#include <dom/dom_element.h>
#include <dom/html_form.h>

#include "bookmarks.h"
#include "xmltree.h"

#include "mainwindowbase.h"
#include "view.h"
#include "htmlview.h"
#include "preferencesimpl.h"
#include "defaults.h"

#include "pics/bar_close.xpm"

int MainWindowBase::s_statusDelay = DEFAULT_STATUSDELAY;
uint MainWindowBase::s_winStyle = DEFAULT_WINSTYLE;
bool MainWindowBase::s_bigPixmaps = DEFAULT_BIGPIXMAPS;
bool MainWindowBase::s_warnClose = false;
QRect *MainWindowBase::s_initGeometry = 0;

MainWindowBase::MainWindowBase( QWidget *parent, const char *name, WFlags flags )
    : QMainWindow( parent, name, flags )
{
}

void MainWindowBase::init()
{
    setUsesBigPixmaps( s_bigPixmaps );

    m_loadImages = 0;

    m_changeable = new QWidget( this );
    m_widgetStack = new QWidgetStack( this, "main widget stack" );
    m_widgetId = 0;

    m_windowList = new QPopupMenu( this, "windows" );
    connect( m_windowList, SIGNAL( aboutToShow() ),
             this, SLOT( prepareWindowListMenu() ) );
    connect( m_windowList, SIGNAL( activated( int ) ),
             this, SLOT( activateWindowFromPopup( int ) ) );

    connect( actionCollection(), SIGNAL( inserted( const char * ) ),
	     this, SLOT( actionChanged( const char * ) ) );
    connect( actionCollection(), SIGNAL( removed( const char * ) ),
	     this, SLOT( actionChanged( const char * ) ) );

    setupZoomMenu();

    setCentralWidget( m_widgetStack );

    setToolBarsMovable( false );

    m_statusBarAutoHideTimer = new QTimer( this, "statusBarAutoHideTimer" );
    connect( m_statusBarAutoHideTimer, SIGNAL( timeout() ),
             statusBar(), SLOT( hide() ) );

    if ( s_statusDelay ) statusBar()->hide();

    m_locationCombo = 0;
    m_windows = 0;

    m_bookmarks = new BookmarkManager( this );

    initActions();
    initGUI();

    if ( m_locationCombo )
    {
	m_locationCombo->setInsertionPolicy( QComboBox::NoInsertion );
        connect( m_locationCombo, SIGNAL( activated( const QString & ) ),
                 this, SLOT( urlEntered( const QString & ) ) );
    }
    m_windows = (QAction*) child( "windows", "QAction" );

    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "General" );
    BrowserView::showLocation = cfg->readBoolEntry( "ShowLocationBar", DEFAULT_LOCATIONBAR );
    BrowserView::showStatusBar = cfg->readBoolEntry( "ShowStatusBar", DEFAULT_STATUSBAR );
    BrowserView::zoomFactor = cfg->readNumEntry( "ZoomFactor", DEFAULT_ZOOM );
    m_downloadURL = cfg->readEntry( "DownloadURL" );

#if defined(ENABLE_SIDEBAR)
    QString sideBarURL = cfg->readEntry( "SideBarURL" );
    int sideBarWidth = cfg->readNumEntry( "SideBarWidth", -162 );
    bool showsb = cfg->readBoolEntry( "ShowSideBar", DEFAULT_SIDEBAR );
    if ( sideBarURL.isEmpty() || abs( sideBarWidth ) < 16 )
    {
	m_sideBar = 0;
	delete m_toggleSideBar;
	m_toggleSideBar = 0;
    }
    else
    {
	QToolBar* tb = new QToolBar( "Side Bar", this, ( sideBarWidth > 0 ) ? Right : Left, false, "sidebar" );
	m_sideBar = new BrowserView( "_sidebar", tb, "htmlview", this, "_sidebar" );
	// This stinks but there's no way to detect the margin width at this
	//  time, especially since we should also account for frame border
	//  which depend on the HTML design. So edit the config file.
	m_sideBar->document()->widget()->setFixedWidth( abs( sideBarWidth ) + 4 );
	tb->setStretchableWidget( m_sideBar->document()->widget() );

	connect( m_sideBar->document()->browserExtension(),
		 SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs & ) ),
		 this, SLOT( createNewView( const KURL &, const KParts::URLArgs & ) ) );
	connect( m_sideBar,
		 SIGNAL( statusMessage( const QString &, bool ) ),
		 this, SLOT( statusJustText( const QString &, bool ) ) );
	connect( m_sideBar->document()->browserExtension(),
		 SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs &,
                                          const KParts::WindowArgs &, KParts::ReadOnlyPart *& ) ),
		 this, SLOT( createNewView( const KURL &, const KParts::URLArgs &,
                             const KParts::WindowArgs &, KParts::ReadOnlyPart *& ) ) );
	connect( m_sideBar, SIGNAL( openURLRequest( View *, const KURL &, const KParts::URLArgs & ) ),
		 this, SLOT( openURL( View *, const KURL &, const KParts::URLArgs & ) ) );

	m_sideBar->openURL( sideBarURL );

	m_toggleSideBar->setChecked( showsb );
	toggleSideBar( showsb );
    }
#endif

    // ### workaround
    (void)new KHTMLFactory( true /*clone*/ );
    KHTMLFactory::defaultHTMLSettings()->init();

    if ( m_autoloadImages )
	m_autoloadImages->setChecked( KHTMLFactory::defaultHTMLSettings()->autoLoadImages() );

    defaultLook();
}

void MainWindowBase::defaultLook()
{
    switch ( s_winStyle )
    {
	case STYLE_WINDOWED:
	    showNormal();
	    if ( s_initGeometry )
		setGeometry( *s_initGeometry );
	    break;
	case STYLE_MAXIMIZED:
	    showMaximized();
	    break;
	case STYLE_FULLSCREEN:
	    showFullScreen();
	    break;
	default:
	    // Have something sensible when returning from full screen
	    s_winStyle = STYLE_WINDOWED;
	    break;
    }
}

MainWindowBase::~MainWindowBase()
{
    if ( m_currentView )
        disconnectView( m_currentView );
    QListIterator<View> it( m_views );
    for ( ; it.current(); ++it )
    {
        disconnect( it.current(), SIGNAL( destroyed() ),
                    this, SLOT( viewDestroyed() ) );
        if ( it.current()->part() )
            disconnect( it.current()->part()->widget(), SIGNAL( destroyed() ),
                        this, SLOT( viewWidgetDestroyed() ) );
    }

    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "General" );
    cfg->writeEntry( "ShowLocationBar", BrowserView::showLocation );
    cfg->writeEntry( "ShowStatusBar", BrowserView::showStatusBar );
#if defined(ENABLE_SIDEBAR)
    if ( m_toggleSideBar )
	cfg->writeEntry( "ShowSideBar", m_toggleSideBar->isChecked() );
#endif
    cfg->writeEntry( "ZoomFactor", BrowserView::zoomFactor );
    cfg->sync();
}

void MainWindowBase::initActions()
{
    debugInit();

#if !defined(KONQ_GUI_KIOSK)

#if defined(ENABLE_SIDEBAR)
    m_toggleSideBar = new KToggleAction( i18n( "Show Side Bar" ), 0,
                        		 this, SLOT( toggleSideBar(bool) ),
					 actionCollection(),
					 "options_show_sidebar" );
#endif // ENABLE_SIDEBAR

    if ( s_winStyle != STYLE_FULLSCREEN )
	new KToggleAction( i18n( "Full Screen Mode" ), CTRL + SHIFT + Key_F,
			   this, SLOT( toggleFullScreen(bool) ),
			   actionCollection(), "view_fullscreen" );

    new KAction( i18n( "Next View" ), CTRL + Key_Tab, this, SLOT( nextView() ),
		 actionCollection(), "view_next" );

    new KAction( i18n( "Close View" ), CTRL + Key_W, this, SLOT( closeView() ),
		 actionCollection(), "view_close" );

    KStdAction::openNew( this, SLOT( newView() ), actionCollection() );

    KStdAction::quit( this, SLOT( close() ), actionCollection() );

    KStdAction::preferences( this, SLOT( showPreferences() ), actionCollection() );

    m_autoloadImages = new KToggleAction( i18n( "Auto Load Images" ), 0,
					  this, SLOT( toggleAutoloadImages(bool) ),
					  actionCollection(), "view_autoload_images" );

    m_closeFind = new QAction( i18n( "Close Find" ), loadPixmap( bar_close_xpm ),
			       QString::null, Key_Escape, m_changeable, "findClose" );

    /*
    m_encodingMenu = new QPopupMenu( this );

    m_encodingMenu->insertItem( QString::fromLatin1( "Auto" ) );

    connect( m_encodingMenu, SIGNAL( activated( const QString & ) ),
             this, SLOT( setEncoding( const QString & ) ) );

    QStringList encodings = KGlobal::charsets()->availableEncodingNames();

    QMap<QString, QStringList> encodingMapping; // languange -> encoding(s)
    QStringList languages;

    QStringList::ConstIterator it = encodings.begin();
    QStringList::ConstIterator end = encodings.end();
    for (; it != end; ++it )
    {
        QString lang = KGlobal::charsets()->languageForEncoding( *it );
        if ( !languages.contains( lang ) )
            languages.append( lang );

        QMap<QString, QStringList>::Iterator mapIt = encodingMapping.find( lang );
        if ( mapIt == encodingMapping.end() )
            mapIt = encodingMapping.insert( lang, QStringList() );

        mapIt.data().append( *it );
    }

    languages.sort();

    it = languages.begin();
    end = languages.end();
    for (; it != end; ++it )
    {
        QPopupMenu *menu = new QPopupMenu( m_encodingMenu );
        m_encodingMenu->insertItem( *it, menu );

        QMap<QString, QStringList>::ConstIterator mapIt = encodingMapping.find( *it );

        if ( mapIt == encodingMapping.end() )
            continue;

        QStringList encodings = mapIt.data();
        encodings.sort();
        QStringList::ConstIterator encodingIt = encodings.begin();
        QStringList::ConstIterator encodingEnd = encodings.end();
        for (; encodingIt != encodingEnd; ++encodingIt )
            menu->insertItem( *encodingIt );
    }

    m_encoding = new PopupAction( m_encodingMenu, QString::null, QIconSet(),
                                  i18n( "Set &Encoding" ), 0, this );
    */
#else // KONQ_GUI_KIOSK
    m_autoloadImages = 0;
    m_closeFind = 0;
#endif // KONQ_GUI_KIOSK
}

QIconSet MainWindowBase::loadPixmap( const char * const xpm[] )
{
    return QIconSet( QPixmap( ( const char ** )xpm ) );
}

void MainWindowBase::urlEntered( const QString &_text )
{
    QString text = _text.stripWhiteSpace();
    if ( text.isEmpty() )
	return;
    KURL u = text;

    if ( u.isMalformed() )
    {
	if ( QRegExp( "^www" ).match( text ) == 0 )
	    u = KURL( text.prepend( "http://" ) );
#if defined(ENABLE_FTP)
	else if ( QRegExp( "^ftp" ).match( text ) == 0 )
	    u = KURL( text.prepend( "ftp://" ) );
#endif
    }

    if ( !u.isMalformed() )
    {
        assert( m_currentView );
        m_currentView->openURL( u );
    }
    else
        KMessageBox::error( this, i18n( "Malformed URL\n%1" ).arg( u.prettyURL() ) );
}

void MainWindowBase::statusJustText( const QString &_msg, bool )
{
    QString msg = _msg;
    msg.replace( QRegExp( "</?b>" ), QString::null );
    msg.replace( QRegExp( "<img.*>" ), QString::null );
#if defined(_QT_QPE_)
    Global::statusMessage( msg );
#else
    statusBar()->message( msg );
#endif
}

void MainWindowBase::statusMessage( const QString &_msg, bool visible )
{
    statusJustText( _msg, visible );
#if !defined(_QT_QPE_)
    if ( visible && s_statusDelay >= 0 )
    {
	statusBar()->show();
	if ( s_statusDelay > 0 )
	    m_statusBarAutoHideTimer->start( s_statusDelay, true );
    }
    else
	statusBar()->hide();
#endif
}

bool MainWindowBase::goSpecialInternal( QString goUrl, const QString &referrer )
{
    int pos = goUrl.find( ' ' );
    KParts::URLArgs args;
    if ( pos >= 0 )
    {
	args.frameName = goUrl.left( pos );
	goUrl.remove( 0, pos+1 );
    }
    if ( goUrl.isEmpty() )
	return false;

    if ( !referrer.isEmpty() )
	args.metaData().insert( "referrer", referrer );

    KURL url( goUrl );

    KParts::BrowserHostExtension *hostExt = 0;
    BrowserView *v = 0;
    if ( !args.frameName.isEmpty() && args.frameName.lower() != "_blank" )
	v = findChildView( args.frameName, &hostExt );
    if ( v )
    {
	setActiveView( v );
	// HACK to prevent changing URL if desired
	if ( ( url.host() != "localhost" ) || url.hasRef() )
	    v->openURL( url, args );
	return false;
    }
    else
    {
	v = createNewViewInternal( url, args );
	KHTMLPartBrowserExtension::setOpenedByJS( v->document(), true );
	return true;
    }
}

bool MainWindowBase::goSpecialURL( const char *name, const QString &referrer )
{
    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "SpecialURLs" );
    return goSpecialInternal( cfg->readEntry( name ), referrer );
}

void MainWindowBase::goSpecialURL()
{
    const char *name = sender()->name( 0 );
    if ( !name )
	return;
    if ( strncmp( name, "url:", 4 ) == 0 )
	name += 4;
    goSpecialURL( name, m_currentView ? m_currentView->locationBarURL() : QString::null );
}

bool MainWindowBase::autoReopen()
{
    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "General" );
    QString url = cfg->readEntry( "AutoReopenURL" );
    return url.isEmpty() ? false : goSpecialInternal( url, QString::null );
}

void MainWindowBase::createNewView( const KURL &url, const KParts::URLArgs &args )
{
    createNewViewInternal( url, args, BrowserView::showStatusBar );
}

void MainWindowBase::addNewView( View *view )
{
    // javascript stuff can do window.close() or shit just happens
    connect( view, SIGNAL( destroyed() ),
             this, SLOT( viewDestroyed() ) );

    connect( view->part()->widget(), SIGNAL( destroyed() ),
             this, SLOT( viewWidgetDestroyed() ) );

    connect( view, SIGNAL( openURLRequest( View *, const KURL &, const KParts::URLArgs & ) ),
             this, SLOT( openURL( View *, const KURL &, const KParts::URLArgs & ) ) );

    connect( view, SIGNAL( createPartView( const KURL &, KParts::Factory *, bool ) ),
	     this, SLOT( createPartView( const KURL &, KParts::Factory *, bool ) ) );

    int id = m_widgetId++;

    m_widgetStack->addWidget( view->part()->widget(), id );
    m_widgetStack->raiseWidget( id );

    // This view order allows a logic and smooth navigation
    bool first = m_views.isEmpty() || ( m_views.at() < 0 );
    m_views.insert( first ? 0 : m_views.at(), view );

    setActiveView( view );
}

BrowserView *MainWindowBase::createNewViewInternal( const KURL &url, const KParts::URLArgs &args, bool withStatus )
{
    KParts::URLArgs newargs(args);
    if ( newargs.frameName.lower() == "_blank" )
	newargs.frameName = "";

    BrowserView *view = new BrowserView( newargs.frameName, m_widgetStack,
					 "htmlview", this, "htmlpart", withStatus );

    addNewView( view );

    connect( view->document()->browserExtension(), SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs & ) ),
             this, SLOT( createNewView( const KURL &, const KParts::URLArgs & ) ) );
    connect( view->document()->browserExtension(), SIGNAL( createNewWindow( const KURL &,
                                                                            const KParts::URLArgs &,
                                                                            const KParts::WindowArgs &,
                                                                            KParts::ReadOnlyPart *& ) ),
             this, SLOT( createNewView( const KURL &, const KParts::URLArgs &,
                                        const KParts::WindowArgs &, KParts::ReadOnlyPart *& ) ) );

    if ( m_locationCombo )
	m_locationCombo->clearEdit();
    if ( !url.isEmpty() )
        view->openURL( url, newargs );

    if ( m_windows ) m_windows->setEnabled( m_views.count() > 1 );

    return view;
}

void MainWindowBase::createPartView( const KURL &url, KParts::Factory *factory, bool rw )
{
    const char *service =  rw ? "KParts::ReadWritePart" : "KParts::ReadOnlyPart";

    KParts::Part *part = factory->createPart( m_widgetStack, service, this, service, service );

    if ( !part )
	return;

    if ( !part->inherits( "KParts::ReadOnlyPart" ) )
    {
	delete part;
	return;
    }

    View *view = new View( m_widgetStack, "partview", (KParts::ReadOnlyPart*)part );

    addNewView( view );

    if ( !url.isEmpty() )
        view->openURL( url );

    if ( m_windows ) m_windows->setEnabled( m_views.count() > 1 );
}

void MainWindowBase::createNewView( const KURL &url, const KParts::URLArgs &args,
				    const KParts::WindowArgs &windowArgs,
				    KParts::ReadOnlyPart *&part )
{
    BrowserView *view = 0;

    if ( !args.frameName.isEmpty() &&
         args.frameName.lower() != "_blank" ) // ### TODO _parent and _top?
    {
        KParts::BrowserHostExtension *hostExtension = 0;
        view = findChildView( args.frameName, &hostExtension );
    }

    if ( !view )
	view = createNewViewInternal( url, args, windowArgs.statusBarVisible );

    // ### obey windowArgs.lowerWindow

    part = view->part();
}

void MainWindowBase::setActiveView( View *view )
{
    if ( m_currentView )
        disconnectView( m_currentView );

    assert( m_views.findRef( view ) != -1 );

    m_currentView = view;

    m_widgetStack->raiseWidget( view->part()->widget() );

    connectView( m_currentView );
    setupView( m_currentView );

    if ( view->caption().isEmpty() )
        setCaption( "Konqueror" );
    else
        setCaption( view->caption() );
}

void MainWindowBase::disconnectView( View *view )
{
    view->actionCollection()->qDisconnect();
    if ( view->part() )
	view->part()->actionCollection()->qDisconnect();

    disconnect( view, SIGNAL( statusMessage( const QString &, bool ) ),
                this, SLOT( statusMessage( const QString &, bool ) ) );

    disconnect( view, SIGNAL( captionChanged( const QString & ) ),
                this, SLOT( setCaption( const QString & ) ) );

    if ( m_locationCombo )
        disconnect( view, SIGNAL( locationBarURLChanged( const QString & ) ),
                    this, SLOT( setLocationBarURL( const QString & ) ) );

    if ( m_findInput )
    {
        disconnect( m_findInput, SIGNAL( textChanged( const QString & ) ),
                    view, SLOT( setFindText( const QString & ) ) );

        disconnect( m_findInput, SIGNAL( returnPressed() ),
                    view, SLOT( findNext() ) );
    }
    if ( view->isPlainBrowser() )
    {
	BrowserView *bview = view->isBrowser();

	disconnect( bview->document(), SIGNAL( popupMenu( KHTMLPart *, const QString & ) ),
		    this, SLOT( popupMenu( KHTMLPart *, const QString & ) ) );

	disconnect( this, SIGNAL( loadImages() ),
		    bview->document(), SLOT( slotLoadImages() ) );
    }

    disconnect( this, SIGNAL( zoomValueSelected( int ) ),
                view, SLOT( zoomTo( int ) ) );

    disconnect( view, SIGNAL( actionChanged( const char * ) ),
                this, SLOT( actionChanged( const char * ) ) );
}

void MainWindowBase::connectView( View *view )
{
    connect( view, SIGNAL( statusMessage( const QString &, bool ) ),
             this, SLOT( statusMessage( const QString &, bool ) ) );

    connect( view, SIGNAL( captionChanged( const QString & ) ),
             this, SLOT( setCaption( const QString & ) ) );

    if ( m_locationCombo )
    {
        connect( view, SIGNAL( locationBarURLChanged( const QString & ) ),
                 this, SLOT( setLocationBarURL( const QString & ) ) );
        setLocationBarURL( view->locationBarURL() );
    }

    if ( m_findInput )
    {
	m_findInput->setText( view->findText() );
        connect( m_findInput, SIGNAL( textChanged( const QString & ) ),
                 view, SLOT( setFindText( const QString & ) ) );

        connect( m_findInput, SIGNAL( returnPressed() ),
                 view, SLOT( findNext() ) );
    }

    if ( view->isPlainBrowser() )
    {
	BrowserView *bview = view->isBrowser();

	connect( bview->document(), SIGNAL( popupMenu( KHTMLPart *, const QString & ) ),
		 this, SLOT( popupMenu( KHTMLPart *, const QString & ) ) );

	connect( this, SIGNAL( loadImages() ),
		 bview->document(), SLOT( slotLoadImages() ) );
    }

    connect( this, SIGNAL( zoomValueSelected( int ) ),
             view, SLOT( zoomTo( int ) ) );

    connect( view, SIGNAL( actionChanged( const char * ) ),
             this, SLOT( actionChanged( const char * ) ) );
}

void MainWindowBase::setupView( View *view )
{
    statusMessage( view->statusText(), view->showStatus() );

#if defined(ENABLE_SIDEBAR)
    QToolBar *sb = toolBar( "sidebar" );
    if ( sb && m_toggleSideBar )
    {
	if ( view->isPlainBrowser() )
	{
	    m_toggleSideBar->setEnabled( true );
	    if ( m_toggleSideBar->isChecked() )
		sb->show();
	}
	else
	{
	    m_toggleSideBar->setEnabled( false );
	    sb->hide();
	}
    }
#endif

    if ( m_autoloadImages ) m_autoloadImages->setEnabled( view->isPlainBrowser() );
    m_bookmarks->enable( view->isPlainBrowser() );

    actionCollection()->qDisconnect();

    layout()->setEnabled( false );

    QAsciiDict<char> visible( 31, true, false );
    QObjectList *list = m_changeable->queryList( "QAction", 0, false, true );
    QObjectListIt it( *list );
    QAction *qa;
    for ( ; ( qa = (QAction*) it.current() ); ++it )
    {
	if ( qa->name( 0 ) && connectAction( qa, view ) )
	    visible.insert( qa->name(), "" );
    }
    delete list;
    list = queryList( "QWidget", 0, false, true );
    it = QObjectListIt ( *list );
    QWidget *tb;
    for ( ; ( tb = (QWidget*) it.current() ); ++it )
    {
	QString name( tb->name( "" ) );
	name.replace( QRegExp( "_action_button$" ), "" );
	if ( !name.contains( '_' ) )
	    continue;

	bool showIt = false;

	if ( tb->inherits( "QToolBar" ) )
	    showIt = name.startsWith( "main" ) || view->itemVisible( name.latin1() );
	else if ( tb->inherits( "QToolButton" ) )
	    showIt |= ( visible.find( name.latin1() ) != 0 );
	else
	    continue;

	if ( showIt )
	    tb->show();
	else
	    tb->hide();
    }
    delete list;

    layout()->setEnabled( true );
    layout()->activate();
}

KAction *MainWindowBase::findAction( const char *name, View *view )
{
    if ( name == 0 || *name == 0 )
	return 0;

    KAction *act = 0;

    if ( view )
    {
	if ( view->part() )
	    act = view->part()->actionCollection()->action( name );
	if ( !act )
	    act = view->actionCollection()->action( name );
    }

    if ( !act )
	act = actionCollection()->action( name );

    return act;
}

bool MainWindowBase::connectAction( QAction *action, View *view )
{
    KAction *act = findAction( action->name( 0 ), view );
    if ( act )
    {
	act->qConnect( action );
	return true;
    }
    if ( QString( action->name( "" ) ).startsWith( "url:" ) )
    {
	// URL buttons are not supposed to belong to an exclusive QActionGroup
	disconnect( action, 0, 0, 0 );
	connect( action, SIGNAL( activated() ),
		 this, SLOT( goSpecialURL() ) );
	action->setEnabled( true );
	return true;
    }
    // HACK: we should disconnect the signals but we cannot since it would
    //  break QActionGroup - so we simply disable it and hope for the best
    action->setEnabled( false );
    return false;
}

void MainWindowBase::actionChanged( const char *name )
{
    if ( !m_currentView )
	return;
    bool visible = false;
    QAction *qa = (QAction*)m_changeable->child( name, "QAction" );
    if ( qa )
    {
	visible = connectAction( qa, m_currentView );
	QString bname( name );
	bname.append( "_action_button" );
	QObjectList *list = queryList( "QToolButton", bname.latin1(), false, true );
	QObjectListIt it ( *list );
	QToolButton *tb;
	for ( ; ( tb = (QToolButton*)(it.current()) ); ++it )
	{
	    if ( visible )
		tb->show();
	    else
		tb->hide();
	    if ( tb->parentWidget() )
		tb->parentWidget()->updateGeometry();
	}
	delete list;
    }
    else
	visible = m_currentView->itemVisible( name );

    QObjectList *list = queryList( 0, name, false, true );
    QObjectListIt it( *list );
    QObject *obj;
    for ( ; ( obj = it.current() ); ++it )
    {
	if ( obj->isWidgetType() )
	    if ( visible )
		((QWidget*)obj)->show();
	    else
		((QWidget*)obj)->hide();
    }
    delete list;
}

void MainWindowBase::prepareWindowListMenu()
{
    m_windowList->clear();

    int i = 0;
    QListIterator<View> it( m_views );
    for (; it.current(); ++it, ++i )
    {
        QString text = it.current()->caption();

        if ( text.isEmpty() )
            text = it.current()->part()->url().prettyURL();

        if ( text.isEmpty() )
            text = i18n( "Untitled %1" ).arg( i + 1 );

        m_windowList->insertItem( text, i );
    }
}

void MainWindowBase::activateWindowFromPopup( int id )
{
    if ( id == -1 )
        return;

    setActiveView( m_views.at( id ) );
}

void MainWindowBase::viewDestroyed()
{
    View *view = const_cast<View *>( static_cast<const View *>( sender() ) );

    assert( view );

    bool current = static_cast<View *>( m_currentView ) == view;

    m_views.removeRef( view );

    if ( m_windows ) m_windows->setEnabled( m_views.count() > 1 );

    // the widgetstack is cleaned up a few calls before

    if ( m_views.count() > 0 && current )
    {
	m_currentView = 0;
        setActiveView( m_views.current() );
    }
    else if ( !m_views.count() )
    {
	m_currentView = 0;
	if ( !autoReopen() )
	    QTimer::singleShot( 0, this, SLOT( close() ) );
    }
}

void MainWindowBase::viewWidgetDestroyed()
{
    // ### I still believe that QWidgetStack should connect to the destroyed signal
    QWidget *widget = const_cast<QWidget *>( static_cast<const QWidget *>( sender() ) );
    assert( widget );

    m_widgetStack->removeWidget( widget );
}

void MainWindowBase::nextView()
{
    if ( m_views.count() > 1 )
    {
	m_views.findRef( m_currentView );
	View *next = m_views.next();
	if ( !next ) next = m_views.first();
	setActiveView( next );
    }
}

void MainWindowBase::closeEvent( QCloseEvent *e )
{
    QListIterator<View> it( m_views );
    for (; it.current(); ++it )
        if ( !it.current()->queryClose() )
	{
	    e->ignore();
	    return;
	}
    e->accept();
}

void MainWindowBase::closeView()
{
    if ( !( m_currentView && m_currentView->queryClose() ) )
        return;

    View *v = m_currentView;

    if ( m_views.count() == 1 )
    {
	if ( !autoReopen() )
	{
	    if ( s_warnClose && KMessageBox::warningYesNo( this,
		 i18n( "Close Konqueror?" ) ) != KMessageBox::Yes )
		return;
	    QTimer::singleShot( 0, this, SLOT( close() ) );
	    return;
	}
    }

    // ### hack
    m_views.findRef( v );
    View *next = m_views.next();
    if ( !next )
        next = m_views.first();

    setActiveView( next );

    delete v->part()->widget();
}

void MainWindowBase::openURL( View *view, const KURL &url, const KParts::URLArgs &_args )
{
    static QString _top = QString::fromLatin1( "_top" );
    static QString _self = QString::fromLatin1( "_self" );
    static QString _parent = QString::fromLatin1( "_parent" );
    static QString _blank = QString::fromLatin1( "_blank" );

    KParts::URLArgs args = _args;
    QString frameName = args.frameName;
    args.frameName = QString::null;

#if defined(ENABLE_SIDEBAR)
    if ( m_sideBar && frameName.lower() == QString::fromLatin1( "_content" ) )
    {
	if ( currentView()->isPlainBrowser() )
	{
	    currentView()->openURL( url, args );
	    return;
	}
	frameName = _blank;
    }
#endif
    if ( frameName.isEmpty() ||
         frameName == _top ||
         frameName == _self ||
         frameName == _parent )
    {
        view->openURL( url, args );
        return;
    }

    if ( frameName == _blank )
    {
        createNewView( url );
        return;
    }

    KParts::BrowserHostExtension *hostExtension = 0;

    if ( view->isBrowser() )
    {
	BrowserView *bview = view->isBrowser();
	if ( bview->canHandleFrame( frameName, &hostExtension ) )
	{
	    if ( hostExtension )
		hostExtension->openURLInFrame( url, args );
	    else
		bview->openURL( url, args );
	}
	else
	{
	    BrowserView *v = findChildView( frameName, &hostExtension );

	    if ( !v )
		createNewView( url, _args );
	    else if ( hostExtension )
		hostExtension->openURLInFrame( url, args );
	    else
		v->openURL( url, args );
	}
    }
    else
	createNewView( url, _args );
}

void MainWindowBase::popupMenu( KHTMLPart *part, const QString &_url )
{
    KURL url, image;
    QString header;
    bool link = false;
    bool frame = false;
    bool jscript = false;

    if ( !_url.isEmpty() )
    {
	link = true;
	url = part->completeURL( _url );
	jscript = ( url.protocol() == "javascript" );
	header = jscript ? i18n( "Javascript" ) : i18n( "Link" );
	if ( url.protocol() == "mailto" )
	    return;
    }
    else
    {
	frame = part->parentPart() != 0;
	header = frame ? i18n( "Frame" ) : i18n( "Page" );
	url = part->url();
    }

    DOM::Element elem = part->nodeUnderMouse();
    if ( !elem.isNull() && ( elem.elementId() == ID_IMG ||
	 ( elem.elementId() == ID_INPUT &&
	   !static_cast<DOM::HTMLInputElement>(elem).src().isEmpty() ) ) )
	image = part->completeURL( elem.getAttribute( "src" ).string() );

    QPopupMenu *menu = new QPopupMenu( this );

    menu->insertItem( i18n( "-- %1 Menu --" ).arg( header ), 0 );
    menu->setItemEnabled( 0, false );
    menu->insertSeparator();

    if ( !jscript )
	menu->insertItem( i18n( "Open in New Window" ), 10 );
    if ( frame )
	menu->insertItem( i18n( "Reload Frame" ), 11 );

    if ( m_bookmarks->isAvailable() )
        menu->insertItem( i18n( "Add Bookmark" ), 20 );

#ifndef QT_NO_CLIPBOARD
    menu->insertSeparator();

    if ( part->actionCollection()->action( "selectAll" ) )
	menu->insertItem( i18n( "Select All" ), 30 );
    if ( part->hasSelection() )
	menu->insertItem( i18n( "Copy Selection" ), 31 );

    menu->insertItem( i18n( "Copy Location" ), 32 );
    if ( !image.isEmpty() )
	menu->insertItem( i18n( "Copy Image Location" ), 33 );
#endif

    KIO::MimeHandler* text_plain = 0;
    if ( !jscript )
    {
	menu->insertSeparator();

	text_plain = (KIO::MimeHandler*)KIO::MimeHandler::Find( QString::fromLatin1( "text/plain" ) );
	if ( text_plain && text_plain->isPart() &&
	     ( !url.isLocalFile() || !url.fileName( false ).isEmpty() ) )
	    menu->insertItem( i18n( "View Source" ), 40 );

	if ( !image.isEmpty() )
	    menu->insertItem( i18n( "View Image" ), 41 );

	if ( !( m_downloadURL.isEmpty() || url.isLocalFile() ) )
	    menu->insertItem( i18n( "Download" ), 50 );
    }

    int id = menu->exec( QCursor::pos() );
    delete menu;

    switch ( id )
    {
	case 10:
	    createNewView( url );
	    break;
	case 11:
	    {
		KParts::URLArgs args;
		args.reload = true;
		part->browserExtension()->setURLArgs( args );
	    }
	    part->openURL( url );
	    break;
	case 20:
	    m_bookmarks->addBookmark( url, _url );
	    break;
#ifndef QT_NO_CLIPBOARD
	case 30:
	    part->actionCollection()->action( "selectAll" )->activate();
	case 31:
	    QApplication::clipboard()->setText( part->selectedText() );
	    break;
	case 32:
	    QApplication::clipboard()->setText( url.url() );
	    break;
	case 33:
	    QApplication::clipboard()->setText( image.url() );
	    break;
#endif
	case 40:
	    createPartView( url, ((KIO::MimePart*)text_plain)->getFactory(),
			    text_plain->isPart()->isReadWrite() );
	    break;
	case 41:
	    createNewView( image );
	    break;
	case 50:
	    goSpecialInternal( m_downloadURL, url.url() );
	    break;
	default:
	    break;
    }
}

BrowserView *MainWindowBase::findChildView( const QString &name, KParts::BrowserHostExtension **hostExtension )
{
    QListIterator<View> it( m_views );
    for ( ; it.current(); ++it )
        if ( it.current()->isBrowser() &&
	     it.current()->isBrowser()->canHandleFrame( name, hostExtension ) )
            return it.current()->isBrowser();

    return 0;
}

void MainWindowBase::newView()
{
    createNewView( KURL() );
}

void MainWindowBase::showPreferences()
{
    Preferences *pref = new Preferences( this );
    if ( pref->exec() == QDialog::Accepted )
    {
        pref->save();

        reparseConfiguration();

        KGlobal::config()->reparseConfiguration();

        kapp->dcopClient()->send( "kcookiejar", "kcookiejar", "reloadPolicy()", QByteArray() );
    }
    delete pref;
}

void MainWindowBase::setLocationBarURL( const QString &url )
{
    if ( !m_locationCombo )
        return;
    if ( url.isEmpty() )
    {
	m_locationCombo->clearEdit();
        return;
    }

    bool exists = false;
    int i = 0;
    for (; i < m_locationCombo->count(); ++i )
        if ( m_locationCombo->text( i ) == url )
        {
            exists = true;
            break;
        }

    if ( exists )
        m_locationCombo->setCurrentItem( i );
    else
    {
        m_locationCombo->insertItem( url, 0 );
        m_locationCombo->setCurrentItem( 0 );
    }
}

void MainWindowBase::toggleSideBar( bool showSideBar )
{
#if defined(ENABLE_SIDEBAR)
    QToolBar *tb = toolBar( "sidebar" );
    if ( !tb )
        return;

    if ( showSideBar )
        tb->show();
    else
        tb->hide();
#endif
}

void MainWindowBase::toggleAutoloadImages( bool autoloadImages )
{
    // ###
    KConfig *config = KGlobal::config();
    config->setGroup( "HTML Settings" );
    config->writeEntry( "AutoLoadImages", autoloadImages );
    //config->sync();

    reparseConfiguration();

    if ( autoloadImages )
    {
	if ( m_loadImages )
	{
	    delete m_loadImages;
	    m_loadImages = 0;
	}
    }
    else if ( !m_loadImages )
	m_loadImages = new KAction( i18n( "Load Images" ), 0,
	    this, SIGNAL( loadImages() ), actionCollection(),
	    "view_load_images" );
}

void MainWindowBase::toggleFullScreen( bool isFullScreen )
{
    if ( isFullScreen )
	showFullScreen();
    else
	defaultLook();
}

void MainWindowBase::zoomMenuAboutToShow()
{
    // ### move all this into a QPopupMenu derived class!
    
    assert( m_currentView );

    int scalingFactor = m_currentView->zoomValue();

    for ( uint index = 0; index < m_zoomMenu->count(); ++index ) {
        int id = m_zoomMenu->idAt( index );
        m_zoomMenu->setItemChecked( id, id == scalingFactor );
    }
}

// ### should be const, but QObject::child isn't :(
QToolBar *MainWindowBase::toolBar( const char *name )
{
    return static_cast<QToolBar *>( child( name, "QToolBar" ) );
}

void MainWindowBase::reparseConfiguration()
{
    QListIterator<View> it( m_views );
    for (; it.current(); ++it )
	if ( it.current()->isBrowser() )
	    static_cast<KHTMLPartBrowserExtension *>( (it.current()->isBrowser() )
		->document()->browserExtension() )->reparseConfiguration();
}

void MainWindowBase::setupZoomMenu()
{
    m_zoomMenu = new QPopupMenu( this, "view_zoom" );

    static const uint scales[] = { 25, 50, 75, 100, 125, 150, 175, 0 };
    for ( const uint *i = scales; *i; i++ ) {
        QString text = QString::number( *i ) + QString::fromLatin1( " %" );
        m_zoomMenu->insertItem( text, *i );
    }

    connect( m_zoomMenu, SIGNAL( activated( int ) ),
             this, SIGNAL( zoomValueSelected( int ) ) );

    connect( m_zoomMenu, SIGNAL( aboutToShow() ),
             this, SLOT( zoomMenuAboutToShow() ) );
}

/*
void MainWindowBase::setEncoding( const QString &encoding )
{
}
*/

PopupAction::PopupAction( QPopupMenu *popup, const QString &text, const QIconSet &iconSet,
                          const QString &menuText, int accel, QObject *parent,
                          const char *name, bool toggle )
    : QAction( text, iconSet, menuText, accel, parent, name, toggle )
{
    m_popup = popup;
    m_toolButton = 0;
    m_popup->installEventFilter( this );
}

bool PopupAction::addTo( QWidget *widget )
{
    if ( widget->inherits( "QToolBar" ) )
    {
        QToolButton *button = new QToolButton( static_cast<QToolBar *>( widget ) );
        button->setToggleButton( isToggleAction() );
	button->setAccel( accel() );
	button->setTextLabel( toolTip() );

        QIconSet icon = iconSet();
        if ( !icon.isNull() )
            button->setIconSet( icon );

        button->setPopup( m_popup );
        button->setPopupDelay( 0 );

        m_toolButton = button;
        return true;
    }
    else if ( widget->inherits( "QPopupMenu" ) )
    {
        QPopupMenu *menu = static_cast<QPopupMenu *>( widget );
        menu->insertItem( menuText(), m_popup );
        return true;
    }
    else if ( widget->inherits( "QMenuBar" ) )
    {
        QMenuBar *menuBar = static_cast<QMenuBar *>( widget );
        menuBar->insertItem( menuText(), m_popup );
        return true;
    }
    return QAction::addTo( widget );
}

void PopupAction::setEnabled( bool enable )
{
    if ( m_toolButton )
        m_toolButton->setEnabled( enable );

    QAction::setEnabled( enable );
}

bool PopupAction::eventFilter( QObject *o, QEvent *e )
{
    if ( o != m_popup || !e || e->type() != QEvent::MouseButtonRelease )
        return false;

    QMouseEvent *mev = static_cast<QMouseEvent *>( e );
    if ( !m_popup->rect().contains( mev->pos() ) )
        return true;

    return false;
}

#include "mainwindow_x11.h"
#include "mainwindow_qpe.h"
#include "mainwindow_kiosk.h"

MainWindowBase *MainWindowBase::instantiate()
{
#if defined(KONQ_GUI_QPE)
    MainWindowBase *mw = new MainWindowQPE();
#elif defined(KONQ_GUI_X11)
    MainWindowBase *mw = new MainWindowX11();
#elif defined(KONQ_GUI_KIOSK)
    MainWindowBase *mw = new MainWindowKiosk();
#else
    return 0;
#endif
    mw->init();
    return mw;
}

#include "mainwindowbase.moc"

/*
 * vim:ts=4:et:sw=4
 */
