#ifndef __dcopclient_h__
#define __dcopclient_h__

#include <qlist.h>
#include <qcstring.h>

class DCOPClientTransaction;

#include "dcopobject.h"
#include "dcopdispatcher.h"

namespace KIO
{
    class Connection;
};

class DCOPClient
{
public:
    // internal hack ;-)
    enum callType { Send, Call };

    DCOPClient();
    ~DCOPClient();

    QCString registerAs( const char *, bool ) { return 0; }

    QCString appId() const { return QCString( "konq-embed" ); }

    bool isApplicationRegistered( const QCString & ) { return true; }

    bool attach() { return true; }

    DCOPClientTransaction *beginTransaction()
        { return DCOPDispatcher::self()->beginTransaction(); }

    void endTransaction( DCOPClientTransaction *transaction, QCString &replyType, QByteArray &replyData )
        { DCOPDispatcher::self()->endTransaction( transaction, replyType, replyData ); }

    bool send( const QCString &remApp, const QCString &remObj, const QCString &remFun,
               const QByteArray &data );

    bool call( const QCString &remApp, const QCString &remObj, const QCString &remFun,
               const QByteArray &data,
               QCString &replyType, QByteArray &replyData );

#if !defined(Q_WS_WIN)
    // extension
    static void setGlobalConnection( KIO::Connection *conn ) { s_connection = conn; }
    static KIO::Connection *globalConnection() { return s_connection; }

private:
    static KIO::Connection *s_connection;
#endif
};

#endif
