#ifndef __kio_slavebase_h__
#define __kio_slavebase_h__

#include <qcstring.h>
#include <qstring.h>

#include <kio/global.h>
#include <kio/authinfo.h>
#include <kio/kprotocolmanager.h>
#include <kurl.h>
#include <dcopclient.h>
#include <kconfig.h>

namespace KIO
{
    class Connection;

    class SlaveBase
    {
    public:
        static SlaveBase *createSlave( const QString &protocol );
        // ### hack until I implement the factories :)
        static bool knownProtocol( const QString &protocol );

        SlaveBase( const QCString &protocol, const QCString &pool_socket = QCString(), const QCString &app_socket = QCString() );
        virtual ~SlaveBase();

        void setConnection( Connection *conn ) { m_connection = conn; }
        Connection *connection() const { return m_connection; }

        void dispatchLoop();

        void data( const QByteArray &data );

        void dataReq();

        virtual void error( int _errid, const QString &_text );

        void connected();

        virtual void finished();

        void slaveStatus( const QString &host, bool connected );

        void statEntry( const UDSEntry &entry );

        void totalSize( unsigned long bytes );

        void processedSize( unsigned long bytes );

        void speed( unsigned long bytesPerSecond );

        void redirection( const KURL &url );

        void errorPage();

        void mimeType( const QString &mimeType );

        virtual void listEntry( const UDSEntry &entry, bool ready );

        void infoMessage( const QString &msg );

        bool openPassDlg( AuthInfo &info );

        enum { QuestionYesNo = 1, WarningYesNo = 2, WarningContinueCancel = 3, WarningYesNoCancel = 4,
               Information = 5, SSLMessageBox = 6 };

        int messageBox( int type, const QString &text,
                        const QString &caption = QString::null,
                        const QString &buttonYes = QString::null,
                        const QString &buttonNo = QString::null );

        void setMetaData( const QString &key, const QString &value );

        bool hasMetaData( const QString &key ) const;

        QString metaData( const QString &key ) const;

        virtual void setHost( const QString &host, int port, const QString &user, const QString &pass );

        virtual void get( const KURL &url );

        virtual void put( const KURL &url, int permissions, bool overwrite, bool resume );

        virtual void stat( const KURL &url );

        virtual void mimetype( const KURL &url );

        virtual void special( const QByteArray & );

        virtual void slave_status();

        virtual void reparseConfiguration();

        int readData( QByteArray &buffer );

        bool checkCachedAuthentication( AuthInfo &info );

        void cacheAuthentication( const AuthInfo &info );

        void sendMetaData();

        void dispatch( int command, const QByteArray &packedArgs );

        void canResume() {}

        KConfig *config() const { return m_kconfig; }

        // ### metadata!
        int connectTimeout() const { return KProtocolManager::connectTimeout(); }

        int proxyConnectTimeout() const { return KProtocolManager::proxyConnectTimeout(); }

        int responseTimeout() const { return KProtocolManager::responseTimeout(); }

        int readTimeout() const { return KProtocolManager::readTimeout(); }

	// ### FIXME!
	void setMultipleAuthCaching( bool enable ) { m_multipleAuthCaching = enable; }
	bool multipleAuthCaching() const { return m_multipleAuthCaching; }

    protected:
        QCString mProtocol;
	    DCOPClient *m_dcc;
        MetaData mOutgoingMetaData;

    private:
        int waitForAnswer( int expected1, int expected2, int *_cmd, QByteArray &data );

        Connection *m_connection;
        MetaData m_incomingMetaData;
        MetaData m_config;
        KConfig *m_kconfig;

	bool m_multipleAuthCaching;
    };

};

/* vim: et sw=4 ts=4
 */

#endif
