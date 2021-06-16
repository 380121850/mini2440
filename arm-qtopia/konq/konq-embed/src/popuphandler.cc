#include "popuphandler.h"

#if defined(_WS_QWS_) && defined(QT_NO_QWS_CURSOR)

#include <qapp.h>
#include <qtimer.h>

PopupHandler::PopupHandler( QApplication *app, const char *name )
    : QObject( app, name )
{
    app->installEventFilter( this );
    m_menuTimer = new QTimer( this );
    connect( m_menuTimer, SIGNAL( timeout() ),
             this, SLOT( popup() ) );

    m_inPopup = false;
}

bool PopupHandler::eventFilter( QObject *obj, QEvent *ev )
{
    if ( !obj || m_inPopup )
        return false;

    if ( ev->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *me = static_cast<QMouseEvent *>( ev );
        if ( me->button() == LeftButton )
        {
            m_obj = obj;
            m_mousePressPos = me->pos();
            m_menuTimer->start( 750, true ); // ### hardcoded
            return false;
	}
    }

    if ( ev->type() == QEvent::MouseButtonRelease ||
         ev->type() == QEvent::Leave ||
         ( ev->type() == QEvent::MouseMove &&
           ( static_cast<QMouseEvent *>( ev )->pos() - m_mousePressPos ).manhattanLength() >
             5 ) ) // ### hardcoded
    {
        m_obj = 0;
        m_mousePressPos = QPoint();
        m_menuTimer->stop();
    }


    return false;
}

void PopupHandler::popup()
{
    if ( !m_obj )
        return;

    QMouseEvent pev( QEvent::MouseButtonPress, m_mousePressPos, RightButton, RightButton );
    QMouseEvent rev( QEvent::MouseButtonRelease, m_mousePressPos, RightButton, RightButton );
    m_menuTimer->stop();
    m_mousePressPos = QPoint();

    m_inPopup = true;

    // send faked mouse event
    QMouseEvent lev( QEvent::MouseButtonRelease, m_mousePressPos, LeftButton, LeftButton );
    qApp->sendEvent( static_cast<QObject *>( m_obj ), &lev );
    qApp->sendEvent( static_cast<QObject *>( m_obj ), &pev );
    qApp->sendEvent( static_cast<QObject *>( m_obj ), &rev );

    m_inPopup = false;
}

#include "popuphandler.moc"

#endif // defined(_WS_QWS_) && defined(QT_NO_QWS_CURSOR)
