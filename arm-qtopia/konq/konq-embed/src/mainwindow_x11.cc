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

// Implementation of the X11 GUI

#include "mainwindow_x11.h"

#if !defined(KONQ_GUI_KIOSK)

#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>

#if defined(ENABLE_XMLUI)
#include "xmltree.h"
#include <kstddirs.h>
#include <kstdaction.h>
#include <qaccel.h>
#include <kdebug.h>

// Helper function
static bool trueAttr( XMLElement *elem, const char *attr )
{
    return elem &&
	   elem->attributes().contains( attr ) &&
	   elem->attributes()[ attr ] == "true";
}

static QString tryTranslate( const QString &str )
{
    // if str contains any non-latin1 character then it's already translated
    for ( unsigned int i = 0; i < str.length(); i++ )
	if ( !str.at( i ).latin1() )
	    return str;
    return QObject::tr( str.latin1() );
}

enum {
    Unknown=0,
    Create,
    Menu,
    ToolBar,
    List
};
#else // ENABLE_XMLUI

#include "pics/new.xpm"
#include "pics/configure.xpm"
#include "pics/next.xpm"
#include "pics/close.xpm"

#if defined(ENABLE_BOOKMARKS)
#include "pics/bookmark.xpm"
#include "pics/bookmark_add.xpm"
#endif

#include <menu.xpm>

#if defined(ENABLE_PRINTING)
#include <print.xpm>
#endif

#include <back.xpm>
#include <forward.xpm>
#include <gohome.xpm>
#include <reload.xpm>
#include <stop.xpm>
#include <find.xpm>
#include <lock.xpm>
#include <viewmag.xpm>
#include <windows.xpm>
#include <images_display.xpm>

#endif // ENABLE_XMLUI

MainWindowX11::MainWindowX11()
    : MainWindowBase( 0, 0, WType_TopLevel|WDestructiveClose )
{
}

void MainWindowX11::initGUI()
{
    QToolBar *mainTb = 0;
    QToolBar *tb = 0;
#if defined(ENABLE_XMLUI)
    XMLElement *ui = XMLElement::load( locate( "data", "konq-embed/konqe.rc" ) );
    if ( ui )
    {
	XMLElement *top = ui->firstChild();
	if ( top && top->tagName() == "kpartgui" )
	    for ( top = top->firstChild(); top; top = top->nextChild() )
	{
	    QString name;
	    if ( top->attributes().contains( "name" ) )
		name = top->attributes()["name"];
	    int sect = Unknown;
	    QPopupMenu *menu = 0;
	    QToolBar *tbar = 0;
	    QActionGroup *list = 0;
	    if ( top->tagName() == "Create" )
		sect = Create;
	    else if ( top->tagName() == "Menu" )
	    {
		if ( !name.isEmpty() )
		    menu = (QPopupMenu*) child( name, "QPopupMenu" );
		sect = Menu;
	    }
	    else if ( top->tagName() == "ToolBar" )
	    {
		if ( !name.isEmpty() )
		{
		    tbar = toolBar( name.latin1() );
		    if ( !tbar )
		    {
#if defined(KONQ_GUI_QPE)
			tbar = new QToolBar( this, name.latin1() );
			moveToolBar( tbar, QMainWindow::Top,
				     trueAttr( top, "newline" ), -1 );
#else
			tbar = new QToolBar( "", this, Top,
					     trueAttr( top, "newline" ), 
					     name.latin1() );
#endif
			if ( trueAttr( top, "fullWidth" ) )
			    tbar->setHorizontalStretchable( true );
			tbar->setBackgroundColor( backgroundColor() );
			if ( name.startsWith( "main" ) && !mainTb )
			    mainTb = tb;
		    }
		}
		sect = ToolBar;
	    }
	    else if ( top->tagName() == "ActionList" )
	    {
		if ( !name.isEmpty() )
		    list = (QActionGroup*) child( name, "QActionGroup" );
		sect = List;
	    }
	    XMLElement *leaf = top->firstChild();
	    for ( ; leaf; leaf = leaf->nextChild() )
	    {
		QString tag = leaf->tagName();
		if ( leaf->attributes().contains( "name" ) )
		    name = leaf->attributes()["name"];
		else
		    name = "";
		switch ( sect )
		{
		    case Create:
			if ( name.isEmpty() || child( name.latin1(), "QAction" ) )
			    continue;
			if ( tag == "Action" )
			{
			    QString text = "";
			    QString menuText = "";
			    QString icon = "";
			    int accel = 0;
			    bool toggle = false;
			    if ( trueAttr( leaf, "stdAction" ) )
			    {
				int stdAct = KStdAction::stdAction( name.latin1() );
				if ( stdAct )
				{
				    menuText = text = KStdAction::stdText( stdAct );
				    accel = KStdAction::stdAccel( stdAct );
				    toggle = KStdAction::stdToggle( stdAct );
				    text.replace( QRegExp( "&" ), "" );
				}
			    }
			    XMLElement *label = leaf->namedItem( "text" );
			    if ( label && label->firstChild())
				text = tryTranslate( label->firstChild()->value() );
			    if ( leaf->attributes().contains( "menuText" ) )
				menuText = tryTranslate( leaf->attributes()["menuText"] );
			    if ( menuText.isEmpty() )
				menuText = text;
			    if ( leaf->attributes().contains( "icon" ) )
				icon = leaf->attributes()["icon"];
			    if ( !( icon.isEmpty() || icon.startsWith( "/" ) ) )
				icon = locate( "data", "konq-embed/actions/" + icon );
			    if ( leaf->attributes().contains( "accel" ) )
			    {
				bool ok = false;
				accel = leaf->attributes()["accel"].toInt( &ok );
				if ( !ok )
				    accel = QAccel::stringToKey( leaf->attributes()["accel"] );
			    }
			    else if ( accel == 0)
				accel = QAccel::shortcutKey( menuText );
			    if ( leaf->attributes().contains( "toggle" ) )
				toggle = trueAttr( leaf, "toggle" );
			    if ( trueAttr( leaf, "popupMenu" ) )
			    {
				QPopupMenu *menu = (QPopupMenu*) child( name.latin1(), "QPopupMenu" );
				if ( !menu )
				    menu = new QPopupMenu( this, name.latin1() );
				new PopupAction( menu, text, QIconSet( QPixmap( icon ) ),
				    menuText, accel, this, name.latin1() );
			    }
			    else
				new QAction( text, QIconSet( QPixmap( icon ) ),
				    menuText, accel, m_changeable,
				    name.latin1(), toggle );
			}
			else if ( tag == "ActionList" )
			{
			    bool exclusive = !leaf->attributes().contains( "exclusive" ) ||
					     leaf->attributes()["exclusive"] == "true";
			    new QActionGroup( m_changeable, name.latin1(), exclusive );
			}
			break;
		    case Menu:
			if ( !menu )
			    continue;
			if ( tag == "Action" && !name.isEmpty() )
			{
			    QAction *act = (QAction*) child( name, "QAction" );
			    if ( act )
				act->addTo( menu );
			}
			else if ( tag == "Separator" )
			    menu->insertSeparator();
			break;
		    case ToolBar:
			if ( !tbar )
			    continue;
			if ( tag == "Action" && !name.isEmpty() )
			{
			    QAction *act = (QAction*) child( name, "QAction" );
			    if ( act )
				act->addTo( tbar );
			}
			else if ( tag == "ToolBar" && !name.isEmpty() )
			{
			    QToolBar *bar = new QToolBar( name, this, tbar, false, name.latin1() );
			    bar->layout()->setMargin( 0 );
			    bar->show();
			}
			else if ( tag == "Spacer" )
			    tbar->setStretchableWidget( new QWidget( tbar, name.latin1() ) );
			else if ( tag == "Separator" )
			    tbar->addSeparator();
			else if ( tag == "Label" && !name.isEmpty() )
			    new QLabel( name, tbar );
			break;
		    case List:
			if ( !list )
			    continue;
			if ( tag == "Action" && !name.isEmpty() )
			{
			    QAction *act = (QAction*) child( name, "QAction" );
			    if ( act )
				list->insert( act );
			}
			break;
		}
	    }
	}
	else
	    kdFatal()<< "Invalid XML UI file" <<endl;
	delete ui;
    }
    else
	kdFatal()<< "Failed to load XML UI file" <<endl;

#else // ENABLE_XMLUI

    mainTb = tb = new QToolBar( this, "mainToolbar" );
    tb->setBackgroundColor( backgroundColor() );
//    tb->setHorizontalStretchable( true );

    QPopupMenu *mm = new QPopupMenu( this );

    QAction *qa;

    qa = new QAction( i18n( "Open new View" ), loadPixmap( new_xpm ),
		      QString::null, CTRL + Key_N, m_changeable, "file_new" );
    qa->addTo( mm );

    mm->insertSeparator();

    qa = new QAction( i18n( "Automatically Load Images" ), QIconSet(),
		      QString::null, 0, m_changeable, "view_autoload_images", true );
    qa->addTo( mm );

    qa = new QAction( i18n( "Show Location Bar" ), QIconSet(),
		      QString::null, 0, m_changeable, "options_show_location", true );
    qa->addTo( mm );

#if defined(ENABLE_SIDEBAR)
    qa = new QAction( i18n( "Show Side Bar" ), QIconSet(),
		      QString::null, 0, m_changeable, "options_show_sidebar", true );
    qa->addTo( mm );
#endif

    mm->insertSeparator();

    qa = new QAction( i18n( "Find in Document" ), loadPixmap( find_xpm ),
		      QString::null, CTRL+Key_F, m_changeable, "edit_find" );
    qa->addTo( mm );

    mm->insertSeparator();

    qa = new QAction( i18n( "Preferences..." ), loadPixmap( configure_xpm ),
		      QString::null, 0, m_changeable, "options_configure" );
    qa->addTo( mm );

    mm->insertSeparator();

#if defined(ENABLE_BOOKMARKS)
    if ( m_bookmarks && m_bookmarks->isAvailable() )
    {
	qa = new QAction( i18n( "Add Bookmark" ), loadPixmap( bookmark_add_xpm ),
			  QString::null, CTRL+Key_B, m_changeable, "bookmark_add" );
	qa->addTo( mm );

	qa = new QAction( i18n( "Edit Bookmarks..." ), loadPixmap( bookmark_xpm ),
			  QString::null, 0, m_changeable, "bookmark_edit" );
	qa->addTo( mm );

	m_bookmarks->addTo( mm );

	mm->insertSeparator();
    }
#endif

    qa = new QAction( i18n( "Next View" ), loadPixmap( next_xpm ),
		      QString::null, CTRL + Key_Tab, m_changeable, "view_next" );
    qa->addTo( mm );

    qa = new QAction( i18n( "Close View" ), loadPixmap( close_xpm ),
		      QString::null, CTRL + Key_W, m_changeable, "view_close" );
    qa->addTo( mm );


    qa = new PopupAction( mm, i18n( "Menu" ), loadPixmap( menu_xpm ),
			  QString::null, 0, this );
    qa->addTo( tb );

#if defined(ENABLE_PRINTING)
    qa = new QAction( i18n( "Print" ), loadPixmap( print_xpm ),
		      QString::null, CTRL+Key_P, m_changeable, "file_print" );
    qa->addTo( tb );
#endif

    qa = new QAction( i18n( "Backward" ), loadPixmap( back_xpm ),
		      QString::null, ALT+Key_Left, m_changeable, "go_back" );
    qa->addTo( tb );

    qa = new QAction( i18n( "Forward" ), loadPixmap( forward_xpm ),
		      QString::null, ALT+Key_Right, m_changeable, "go_forward" );
    qa->addTo( tb );

    qa = new QAction( i18n( "Home Page" ), loadPixmap( gohome_xpm ),
		      QString::null, CTRL+Key_Home, m_changeable, "go_home" );
    qa->addTo( tb );

    qa = new QAction( i18n( "Reload" ), loadPixmap( reload_xpm ),
		      QString::null, Key_F5, m_changeable, "view_redisplay" );
    qa->addTo( tb );

    qa = new QAction( i18n( "Stop" ), loadPixmap( stop_xpm ),
		      i18n( "Stop Loading" ), 0, m_changeable, "go_stop" );
    qa->addTo( tb );

    qa = new QAction( i18n( "Show Security Information" ), loadPixmap( lock_xpm ),
		      QString::null, 0, m_changeable, "info_security" );
    qa->addTo( tb );

    qa= new PopupAction( m_zoomMenu, i18n( "Zoom" ), loadPixmap( viewmag_xpm ),
			 i18n( "Zoom Document" ), 0, this );
    qa->addTo( tb );

    qa = new QAction( i18n( "Load Images" ), loadPixmap( images_display_xpm ),
		      QString::null, 0, m_changeable, "view_load_images" );
    qa->addTo( tb );

    qa = new PopupAction( m_windowList, i18n( "Window List" ), loadPixmap( windows_xpm ),
			  QString::null, 0, this );
    qa->addTo( tb );

#endif // ENABLE_XMLUI

    if ( mainTb )
	mainTb->show();

#if defined(KONQ_GUI_QPE)
    tb = new QToolBar( this, "location_bar" );
    tb->setLabel( i18n( "Location:" ) );
    moveToolBar( tb, QMainWindow::Top, true, -1 );
#else
    tb = new QToolBar( i18n( "Location:" ), this, QMainWindow::Top, true, "location_bar" );
#endif
    tb->setHorizontalStretchable( true );
    m_locationCombo = new QComboBox( true, tb, "locationcombo" );
    tb->show();

    statusBar()->setSizeGripEnabled( false );

#if defined(KONQ_GUI_QPE)
    tb = new QToolBar( this, "search_bar" );
    moveToolBar( tb, QMainWindow::Top, true, -1 );
#else
    tb = new QToolBar( "searchbar", this, QMainWindow::Top, true, "search_bar" );
#endif
    m_findInput = new QLineEdit( tb, "search" );
    tb->setStretchableWidget( m_findInput );

    m_closeFind->addTo( tb );
    QToolButton *btn = (QToolButton *) tb->child( 0, "QToolButton" );
    if ( btn )
	btn->setUsesBigPixmap( FALSE );

    tb->hide();
}

#include "mainwindow_x11.moc"

#endif // KONQ_GUI_KIOSK

/*
 * vim:ts=4:et:sw=4
 */
