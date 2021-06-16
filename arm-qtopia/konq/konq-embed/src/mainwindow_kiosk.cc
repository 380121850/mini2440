// Implementation of the Kiosk GUI

#include "mainwindow_kiosk.h"

#if defined(KONQ_GUI_KIOSK)

#include "view.h"
#include "htmlview.h"

MainWindowKiosk::MainWindowKiosk()
    : MainWindowBase( 0, 0, WType_TopLevel|WDestructiveClose|WStyle_Customize|WStyle_NoBorderEx )
{
}

View *MainWindowKiosk::createNewViewInternal( const KURL &url, const KParts::URLArgs &args )
{
    BrowserView *v = MainWindowBase::createNewViewInternal( url, args );
    v->document()->view()->setVScrollBarMode( QScrollView::AlwaysOff );
    v->document()->view()->setHScrollBarMode( QScrollView::AlwaysOff );
    return v;
}

void MainWindowKiosk::initGUI()
{
    m_findInput = 0;
}

void MainWindowKiosk::popupMenu( const QString &_url, const QPoint & )
{
	// There're no popup menus for the kiosk
}

#include "mainwindow_kiosk.moc"

#endif
