#ifndef __channel_h__
#define __channel_h__

#include <qglobal.h>
#if defined(Q_WS_WIN32)

#include <qthread.h>
#include <qguardedptr.h>

class Channel;

class ChannelMessage
{
    friend class Channel;
public:
    ChannelMessage( int type = -1 );
    virtual ~ChannelMessage();

    int type() const { return m_type; }
    
    void reply();

private:
    ChannelMessage( const ChannelMessage & );
    ChannelMessage &operator=( const ChannelMessage );

    int m_type;
    bool m_isCall : 1;
    bool m_replied : 1;
    bool m_inEventHandler : 1;
    QMutex m_guard;
    QWaitCondition m_condition;
    QGuardedPtr<Channel> m_channel;
};

class Channel : public QObject
{
    Q_OBJECT
public:
    enum SendType { OneWay, WaitForReply };
    Channel( QObject *parent = 0, const char *name = 0 );
    virtual ~Channel();

    void send( ChannelMessage *message, SendType type );

protected:
    virtual void receiveMessage( ChannelMessage *message, SendType type ) = 0;
    
    virtual bool event( QEvent *ev );

private:
    struct MsgEnvelope
    {
	MsgEnvelope( SendType _type , ChannelMessage *_msg )
	    : type( _type ), msg( _msg ) {}
	SendType type;
	ChannelMessage *msg;
    };
};

#endif

#endif
