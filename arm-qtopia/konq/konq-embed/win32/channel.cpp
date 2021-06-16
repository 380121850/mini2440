
#include "channel.h"
#include "autolock.h"

#include <assert.h>

ChannelMessage::ChannelMessage( int type )
    : m_type( type ), m_isCall( false ), m_replied( false ),
      m_inEventHandler( false )
{
}

ChannelMessage::~ChannelMessage()
{
    if ( m_guard.locked() )
	m_guard.unlock();
}

void ChannelMessage::reply()
{
    if ( !m_isCall )
    {
	qDebug( "ChannelMessage::reply() - can't reply oneway message!" );
	return;
    }

    if ( m_inEventHandler )
    {
	m_replied = true;
	return;
    }

    m_condition.wakeOne();
    m_guard.unlock();
}

Channel::Channel( QObject *parent, const char *name )
    : QObject( parent, name )
{
}

Channel::~Channel()
{
}

void Channel::send( ChannelMessage *message, SendType type )
{
    if ( type == WaitForReply )
    {
	message->m_guard.lock();
	message->m_isCall = true;
    }

    MsgEnvelope *envelope = new MsgEnvelope( type, message );
    QThread::postEvent( this, new QCustomEvent( QEvent::User, envelope ) );

    if ( type == WaitForReply )
    {
	message->m_condition.wait( &message->m_guard );
	message->m_guard.unlock();
    }
}

bool Channel::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
	QCustomEvent *ce = static_cast<QCustomEvent *>( ev );
	MsgEnvelope *envelope = static_cast<MsgEnvelope *>( ce->data() );
	ChannelMessage *msg = envelope->msg;

	assert( msg );
	
	if ( envelope->type == WaitForReply )
	{
	    msg->m_guard.lock();
	    msg->m_inEventHandler = true;
	}

	receiveMessage( msg, envelope->type );

	if ( envelope->type == WaitForReply )
	{
	    msg->m_inEventHandler = false;
	    if ( msg->m_replied )
	    {
		msg->m_condition.wakeOne();
		// this is a bit tricky. we unlock only when we reply.
		// reply() does an unlock as well.
		msg->m_guard.unlock();
	    }
	}

	delete envelope;
	return true;
    }
    return QObject::event( ev );
}
