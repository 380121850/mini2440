#ifndef __dcopdispatcher_h__
#define __dcopdispatcher_h__

#include <qcstring.h>
#include <qlist.h>
#include <qobject.h>
#include <qvaluelist.h>
#include <qasciidict.h>
#include <qthread.h>
#include <dcopobject.h>
#if defined(Q_WS_WIN32)
#include <channel.h>
#endif

class DCOPDispatcherChannel;
class QSocketNotifier;

namespace KIO
{
    class Connection;
};

class DCOPClientTransaction;

class DCOPDispatcher : public QObject
{
    Q_OBJECT
    friend class DCOPDispatcherChannel;
public:
    DCOPDispatcher();
    virtual ~DCOPDispatcher();

    DCOPClientTransaction *beginTransaction();
    void endTransaction( DCOPClientTransaction *transaction, QCString &replyType, QByteArray &replyData );

#if defined(Q_WS_WIN)
    bool process( Channel::SendType sendType, const QCString &appname,
		  const QCString &fun, const QByteArray &data,
		  QCString &replyType, QByteArray &replyData );
#else
    // extension
    void addClient( KIO::Connection *connection, QObject *owner );

    bool process( const QCString &appname,
		  const QCString &fun, const QByteArray &data,
		  QCString &replyType, QByteArray &replyData );
#endif

    void registerObject(DCOPObject *obj);

    void unregisterObject(DCOPObject *obj);

    static DCOPDispatcher *self();

#if defined(Q_WS_WIN)
protected:
    virtual void receiveMessage( ChannelMessage *message, Channel::SendType type );

private:
    struct Request : public ChannelMessage
    {
	Request( const QCString &_appname, const QCString &_fun,
		 const QByteArray &_data )
	    : appname( _appname ), fun( _fun ), data( _data )
	{
	    appname.detach();
	    fun.detach(); 
	    data.detach();
	}

	bool send;
	QCString appname;
	QCString fun;
	QByteArray data;
	bool result;
	QCString replyType;
	QByteArray replyData;
    };

    Request *m_currentRequest;
    QList<Request> m_transactions;
    bool m_transactionAdded;

    QMutex m_objpoolGuard;

    uint m_guiThread;

    DCOPDispatcherChannel *m_channel;
#else
private:
    struct Client
    {
        KIO::Connection *m_connection;
        QObject *m_owner;
        QSocketNotifier *m_notifier;
        int m_infd;
    };
    QList<Client> m_clients;

    void disconnectClient( const QObject *owner );
    void dispatchClient( Client *client );

    struct InternalDCOPClientTransaction
    {
        Client *m_client;
    };

    QList<InternalDCOPClientTransaction> m_transactions;

    bool m_transactionAdded;

    static Client *s_currentClient;

#endif
private slots:
    void slotDisconnectClient();
    void slotDispatch( int sockfd );

private:
    QAsciiDict<DCOPObject> m_objpool;

    static DCOPDispatcher *s_self;
};

#endif
