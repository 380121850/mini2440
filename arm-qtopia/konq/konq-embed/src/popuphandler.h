#ifndef __popuphandler_h__
#define __popuphandler_h__

#if defined(_WS_QWS_) && defined(QT_NO_QWS_CURSOR)

#include <qobject.h>
#include <qguardedptr.h>

class QApplication;
class QTimer;

class PopupHandler : public QObject
{
    Q_OBJECT
public:
    PopupHandler( QApplication *app, const char *name );

    virtual bool eventFilter( QObject *obj, QEvent *ev );

private slots:
    void popup();

private:
    bool m_inPopup;
    QTimer *m_menuTimer;
    QGuardedPtr<QObject> m_obj;
    QPoint m_mousePressPos;
};

#endif // defined(_WS_QWS_) && defined(QT_NO_QWS_CURSOR)

#endif
