#ifndef __kio_krun_h__
#define __kio_krun_h__

#include <qobject.h>
#include <kurl.h>
#include <qtimer.h>

namespace KIO
{
    class Job;
};

class KRun : public QObject
{
    Q_OBJECT
public:
    KRun( const KURL &, int, bool, bool );
    ~KRun();

    static void runURL( const KURL& _url, const QString& _mimetype );

    KIO::Job *job() const { return m_job; }

signals:
    void error();

protected:
    void scanFile() {}

    virtual void foundMimeType( const QString &type );

    KURL m_strURL;
    bool m_bFinished;
    QTimer m_timer;
    KIO::Job *m_job;

protected slots:
    virtual void slotJobResult( KIO::Job *job );
    virtual void slotScanFinished( KIO::Job * ) {}

private slots:
    void slotStart();
    void slotJobData( KIO::Job *job, const QByteArray &data );
    void slotFoundMime( KIO::Job *job, const QString &type );

private:
    int m_fhandle;
    static int exec( const QString &app, const QString &arg, const QString &ref = QString::null, bool passdata = false );
};

#endif
