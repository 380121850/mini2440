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

*/

// !! The history code is (c) David Faure <faure@kde.org> !!

#ifndef __mainwindowbase_h__
#define __mainwindowbase_h__

#include <qmainwindow.h>
#include <qaction.h>

#include <kurl.h>
#include <kparts/browserextension.h>
#include <kparts/factory.h>
#include <kaction.h>

class QComboBox;
class QLineEdit;
class QTimer;
class QWidgetStack;
class QRect;
class View;
class BrowserView;
class KHTMLPart;
class QToolButton;
class BookmarkManager;

#include "bookmarks.h"

namespace KIO
{
    class Job;
};

class MainWindowBase : public QMainWindow
{
    Q_OBJECT
public:
    enum {
	STYLE_QTDEFAULT=0,
	STYLE_WINDOWED=1,
	STYLE_MAXIMIZED=2,
	STYLE_FULLSCREEN=3
    };
    enum {
	SHOW_TOOLBAR=0x0001,
	SHOW_LOCATION=0x0002,
	SHOW_STATUS=0x0004,
	SHOW_SIDEBAR=0x0008,
	SHOW_ALL=0x000f,
	SHOW_NOSAVE=0x8000,
	SHOW_UTILITY=0xff00
    };
    virtual ~MainWindowBase();

    View *currentView() const { return m_currentView; }

    static MainWindowBase *instantiate();

    QObject *child( const QString &name, const char *type = 0 )
	{ return QMainWindow::child( name.latin1(), type ); }

    bool goSpecialURL( const char *name, const QString &referrer = QString::null );

public slots:
    void createNewView( const KURL &url, const KParts::URLArgs &args = KParts::URLArgs() );
    void createNewView( const KURL &url, const KParts::URLArgs &args,
                        const KParts::WindowArgs &windowArgs, KParts::ReadOnlyPart *&part );

    void createPartView( const KURL &url, KParts::Factory *factory, bool rw );
    void urlEntered( const QString & );

protected:
    MainWindowBase( QWidget *parent, const char *name, WFlags flags );

    virtual void closeEvent( QCloseEvent *e );

    virtual void init();

    void defaultLook();

    bool connectAction( QAction *action, View *view = 0 );

    void debugInit();

protected slots:
    void statusJustText( const QString &msg, bool visible );

    void statusMessage( const QString &msg, bool visible );

    void prepareWindowListMenu();

    void activateWindowFromPopup( int id );

    void viewDestroyed();

    void viewWidgetDestroyed();

    void nextView();

    void closeView();

    void openURL( View *view, const KURL &url, const KParts::URLArgs &args );

    void popupMenu( KHTMLPart *part, const QString &_url );

    void newView();

    void showPreferences();

    void setLocationBarURL( const QString &url );

//    void setEncoding( const QString &encoding );

    void toggleAutoloadImages( bool autoloadImages );

    void toggleFullScreen( bool isFullScreen );

    void zoomMenuAboutToShow();

    void goSpecialURL();

    void toggleSideBar( bool showSideBar );

    void actionChanged( const char *name );

    void debugFunc();

signals:
    void zoomValueSelected( int );

    void loadImages();

protected:
    virtual BrowserView *createNewViewInternal( const KURL &url, const KParts::URLArgs &args, bool withStatus = true );

    bool goSpecialInternal( QString goUrl, const QString &referrer = QString::null );

    void addNewView( View *view );

    void setActiveView( View *view );

    void disconnectView( View *view );

    void connectView( View *view );

    void setupView( View *view );

    bool autoReopen();

    KAction *findAction( const char *name, View *view = 0 );

    BrowserView *findChildView( const QString &name, KParts::BrowserHostExtension **hostExtension );

    void initActions();

    virtual void initGUI() = 0;

    virtual QIconSet loadPixmap(  const char * const xpm[] );

    QToolBar *toolBar( const char *name );

    void reparseConfiguration();

    void setupZoomMenu();

    KActionCollection m_collection;

    KAction *m_loadImages;
    KToggleAction *m_autoloadImages;

    QAction *m_closeFind;
    QAction *m_windows;

    QToolBar *m_searchBar;
    QLineEdit *m_findInput;

    QComboBox *m_locationCombo;

#if defined(ENABLE_SIDEBAR)
    KToggleAction *m_toggleSideBar;
    BrowserView *m_sideBar;
#endif

    QWidget *m_changeable;

    QPopupMenu *m_windowList;
    QPopupMenu *m_zoomMenu;

//    QPopupMenu *m_encodingMenu;
//    QAction *m_encoding;

    QString m_downloadURL;

    BookmarkManager *m_bookmarks;

    QTimer *m_statusBarAutoHideTimer;

    QWidgetStack *m_widgetStack;
    QList<View> m_views;
    QGuardedPtr<View> m_currentView;
    int m_widgetId;

public:
    static int s_statusDelay;
    static uint s_winStyle;
    static bool s_bigPixmaps;
    static bool s_warnClose;
    static QRect *s_initGeometry;

    KActionCollection *actionCollection() { return &m_collection; }
};

// destruction and removeFrom code I deliberated left out as it's not needed
// here and just costs size :)
class PopupAction : public QAction
{
    Q_OBJECT
public:
    PopupAction( QPopupMenu *popup, const QString &text, const QIconSet &iconSet,
                 const QString &menuText, int accel, QObject *parent, const char *name = 0,
                 bool toggle = false );

    virtual bool addTo( QWidget *widget );

public slots:
    virtual void setEnabled( bool enable );

protected:
    virtual bool eventFilter( QObject *o, QEvent *e );

private:
    QPopupMenu *m_popup;
    QToolButton *m_toolButton; // ### let's save memory and assume we plug the action only once anyway
};

#endif
