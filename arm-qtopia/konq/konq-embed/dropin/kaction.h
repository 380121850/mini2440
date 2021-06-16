#ifndef __kaction_h__
#define __kaction_h__

#include <qobject.h>
#include <qobjectlist.h>
#include <qguardedptr.h>
#include <qmap.h>

class QIconSet;
class KAction;
class QAction;

class KActionCollection : public QObject
{
    Q_OBJECT
public:
    KActionCollection( QObject *parent = 0, const char *name = 0 )
        : QObject( parent, name ), m_timer( true ) {}

    KAction *action( const char *name );

    virtual void insertChild( QObject *obj );
    virtual void removeChild( QObject *obj );
    inline void insert( KAction *act ) { insertChild( (QObject*)act ); }
    inline void remove( KAction *act ) { removeChild( (QObject*)act ); }

    void qDisconnect();
    void clearNotifications() { m_newlyAdded.clear(); }

signals:
    void inserted( KAction* );
    void inserted( const char * );
    void removed( KAction* );
    void removed( const char * );

private slots:
    void insertNotify();

private:
    bool m_timer;
    QObjectList m_newlyAdded;
};

class KAction : public QObject
{
    Q_OBJECT
public:
    KAction( const QString &text, int accel, const QObject *receiver, const char *slot,
             QObject *parent, const char *name, bool connectIt = true );

    KAction( const QString &text, const QString &icon, int accel, const QObject *receiver, const char *slot,
             QObject *parent, const char *name );

    KAction( const QString &text, const QIconSet &icon, int accel, const QObject *receiver, const char *slot,
             QObject *parent, const char *name );

    virtual ~KAction();

    void setIcon( const QString & ) {};

    bool isEnabled() const { return m_enabled; }
    void setText( const QString& ) {};

    // assigns an icon name (used with setIcon) a specified iconset
    void assignIconSet( const QString &, const QIconSet & ) {};

    virtual void qConnect( QAction *action );
    virtual void qDisconnect();

public slots:
    void setEnabled( bool enable );
    void activate() { emit activated(); }

signals:
    void activated();
    void enabled( bool );

protected:
    bool m_enabled;
    QGuardedPtr<QAction> m_qConnected;
};

class KSelectAction : public KAction
{
public:
    KSelectAction( const QString &text, int accel, const QObject *receiver, const char *slot,
                   QObject *parent, const char *name )
	: KAction( text, accel, receiver, slot, parent, name ) {}

    // ###
    void setItems( const QStringList & ) {}
    void setCurrentItem( int ) {}

    int currentItem() { return 0; }
    QString currentText() { return QString::null; }
};

class KToggleAction : public KAction
{
    Q_OBJECT
public:
    KToggleAction( const QString &text, int accel,
		   const QObject *receiver, const char *slot,
        	   QObject *parent, const char *name, bool connectIt = true );

    KToggleAction( const QString &text, int accel,
        	   QObject *parent, const char *name );

    bool isChecked() const { return m_checked; }

    virtual void qConnect( QAction *action );
    virtual void qDisconnect();

public slots:
    void setChecked( bool checked );

signals:
    void toggled( bool );

protected slots:
    void slotChecked( bool checked );

protected:
    bool m_checked;
    bool m_reenter;
};

#endif
