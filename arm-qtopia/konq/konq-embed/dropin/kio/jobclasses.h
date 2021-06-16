#ifndef __kio_job_h__
#define __kio_job_h__

class QWidget;

#include <qobject.h>
#include <qcstring.h>

#include <kurl.h>

#include <kio/global.h>
#include <kio/mimehandler.h>

namespace KIO
{
    class Slave;
    class TransferJob;

    class Job : public QObject
    {
        Q_OBJECT
    protected:
        Job( bool showProgressInfo );
    public:
        virtual ~Job();

        virtual void kill();

        void setWindow( QWidget *window ) { m_widget = window; }
        QWidget *window() const { return m_widget; }

        int error() const { return m_error; }

        QString errorText() { return m_errorText; }

        QString errorString();

        void showErrorDialog( QWidget *parent = 0 );

        void putOnHold() {} // ### check whether this belongs into another job class

	void setTaskNumber(int task) { m_task = task; }
	int taskNumber() const { return m_task; }

    signals:
        void result( KIO::Job *job );

        void infoMessage( KIO::Job *, const QString &msg );

        void connect( KIO::Job * );

        void percent( KIO::Job *job, unsigned long percent );

        void totalSize( KIO::Job *, unsigned long size );

        void processedSize( KIO::Job *, unsigned long size );

        void speed( KIO::Job *, unsigned long bytesPerSecond );

    protected:

        int m_error;
	unsigned long m_percent;
        QString m_errorText;
	void emitPercent( unsigned long size, unsigned long total );
	inline void recurseEnter() { m_reenter++; }
	bool recurseExit();
	bool isDestructing() const { return m_destruct; }

    private:
        QWidget *m_widget;
        bool m_showProgressInfo;
	int m_task;
	int m_reenter;
	bool m_destruct;
    };

    class SimpleJob : public KIO::Job
    {
        Q_OBJECT
    public:
        SimpleJob( const KURL &url, int command,
                   const QByteArray &packedArgs,
                   bool showProgressInfo );
        virtual ~SimpleJob();

        KURL url() const { return m_url; }

        virtual void start( Slave *slave );

        virtual void kill();

     public slots:

	  void slotProcessedSize( unsigned long );
	  void slotTotalSize( unsigned long );
	  void slotPercent( unsigned long );

     protected slots:
        virtual void receiveData( const QByteArray &dat );

        virtual void slaveFinished();

        virtual void dataReq();

        virtual void slaveRedirection( const KURL &url );

        virtual void slotInfoMessage( const QString &msg );

        virtual void slotError( int id, const QString &text );

    protected:
        Slave *m_slave;

        KURL m_url;
        int m_command;
        QByteArray m_packedArgs;
        unsigned long m_totalSize;
    };

    class TransferJob : public KIO::SimpleJob
    {
        Q_OBJECT
    public:
        TransferJob( const KURL &, int command,
                     const QByteArray &packedArgs,
                     const QByteArray &_staticData,
                     bool showProgressInfo );

        virtual ~TransferJob();

        void setMetaData( const KIO::MetaData &data );

        void addMetaData( const QString &key, const QString &value );

        void addMetaData( const QMap<QString,QString> &values );

        MetaData metaData();

        MetaData outgoingMetaData();

        QString queryMetaData( const QString &key );

        bool isErrorPage() { return false; }

        virtual void start( Slave *slave );

        virtual void kill();

        void detach( const QByteArray &cachedData = QByteArray() );
        void attach();

        static TransferJob *findDetachedJobForURL( const KURL &url );

	QString mimetype() const { return m_mimetype; }

	bool AddFilter( const MimeFilter* filter );

     protected slots:
        void setIncomingMetaData( const KIO::MetaData &dat );

        virtual void dataReq();

        virtual void slaveRedirection( const KURL &url );

        virtual void slaveFinished();

        virtual void receiveData( const QByteArray &dat );

        virtual void filteredData( const QByteArray &dat );

	virtual void slotMimetype( const QString& type );

    signals:
        void redirection( KIO::Job *, const KURL & );

        void data( KIO::Job *job, const QByteArray &data );

	void mimetype( KIO::Job *, const QString &type );

    private slots:
        void slotRedirectDelayed();
        void slotEmitCachedData();

    private:
        MetaData m_outgoingMetaData;
        MetaData m_incomingMetaData;
        QByteArray m_staticData;
        KURL::List m_redirectionList;
        KURL m_redirectionURL;
        bool m_detached;
	bool m_finishAfterCacheEmit;
        QValueList<QByteArray> m_cachedData;
        bool m_cachedDataEmitted;
	QString m_mimetype;

        static QList<TransferJob> *s_detachedJobs;
	DataFilter *m_filter;
	bool m_firstdata;
	void mimeRules( const MimeHandler* mime );
    };

};

#endif
