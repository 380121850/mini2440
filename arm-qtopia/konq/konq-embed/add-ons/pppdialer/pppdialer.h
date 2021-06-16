#ifndef __PPPDIALER_H
#define __PPPDIALER_H

#define NOTIFY_SIGNAL SIGUSR1

#include "dialer.h"
#include <kaction.h>
#include <qstringlist.h>

class QTimer;
class KSocket;

class PPPInitializer : public QObject
{
    Q_OBJECT
public:
    PPPInitializer();

private slots:
    void runInit();
};

class KPPPAction : public KAction
{
    Q_OBJECT
friend class PPPInitializer;

public:
    enum {
	NoConnection = 0,
	Connecting = 1,
	Established = 2,
	_first = NoConnection,
	_last = Established
    };
    virtual ~KPPPAction();
    void setCurrentStatus( int status );
    inline int getCurrentStatus() const { return m_lastStat; }
    const QString getStatusText() const;
    inline const QString &getCurrentText() const { return m_lastText; }
    void interpretText( const QString &text );
    void attemptConnect( const QString &account );
    void attemptDisconnect();
    void listAccounts();
    const QStringList &getAccounts() const { return m_accounts; }

signals:
    void accountsAvailable();

private:
    KPPPAction();
    void fixConnectStatus();
    static bool createInstance();
    static void signalHandler( int sigtype );
    static KPPPAction *s_action;

private slots:
    void showDialer();
    void signalEvent();
    void statusRefresh();
    void readEvent( KSocket *sock );
    void closeEvent( KSocket *sock );

protected:
    bool scriptExecute( const char *command, const char *arg = 0);

    int m_lastStat;
    QString m_lastText;
    bool m_connect;
    bool m_auto;
    QTimer *m_refresh;
    QStringList m_accounts;

    static QPixmap *s_images[3];
    static QString s_script;
    static int s_timeout;
};

class DialerDlg : public DialerBase
{
    Q_OBJECT
public:
    DialerDlg( KPPPAction *action, QWidget* parent = 0 );
    void setMode( bool connected );
    void setData( const QString &text, const QString &caption = QString::null );

protected:
    QLabel* m_text;
    KPPPAction *m_kaction;

public slots:
    void autoClose() { reject(); }

protected slots:
    virtual void accept();
    void insertAccounts();
};

#endif // __PPPDIALER_H
