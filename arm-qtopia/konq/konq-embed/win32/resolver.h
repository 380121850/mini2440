#ifndef __resolver_h__
#define __resolver_h__

#include <qhostaddress.h>
#include <qdns.h>
#include <qptrdict.h>
#include <qthread.h>
#include <channel.h>

// QDns uses timers and the like. the qt thread documentation says not
// to use these in a thread as they might not receive events from the
// underlying windowing system. so instead let's resolve names in the
// event thread and pass it back to the calling thread. resolve() is
// (deliberately) blocking.

// internal
struct NameRequest : public ChannelMessage
{
    NameRequest( const QString &name ) : hostName( name ), resolved( false ) {}
    QString hostName;
    bool resolved;
    QHostAddress address;
};

class Resolver : public Channel
{
    Q_OBJECT
public:
    static void initStatic();

    static Resolver *self();

    bool resolve( const QString &hostName, QHostAddress &address );

protected:
    virtual void receiveMessage( ChannelMessage *message, Channel::SendType );

    static Resolver *s_self;
};

// avoiding the use of sender() ......
// ( ### can't make it a child class of Resolver because moc gets confused :-(
class ResolveHelper : public QObject
{
    Q_OBJECT
public:
    ResolveHelper( NameRequest *request );
    
private slots:
    void nameResolved();
    void deleteMe();

private:
    QDns m_dns;
    NameRequest *m_request;
};

#endif
