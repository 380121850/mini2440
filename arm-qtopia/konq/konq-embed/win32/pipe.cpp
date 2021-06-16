
#include "pipe.h"
#include "channel.h"

#include <assert.h>

Pipe::Pipe( QObject *parent, const char *name )
    : Channel( parent, name )
{
}

Pipe::~Pipe()
{
}

void Pipe::write( const QByteArray &input )
{
    m_queueMutex.lock();
    QByteArray data = input;
    data.detach();
    m_queue.append( data );
    send( new ChannelMessage, Channel::OneWay );
    m_readCondition.wakeOne();
    m_queueMutex.unlock();
}

QByteArray Pipe::read()
{
    m_queueMutex.lock();
    while ( m_queue.isEmpty() )
	m_readCondition.wait( &m_queueMutex );
    QValueList<QByteArray>::Iterator it = m_queue.begin();
    QByteArray result = *it;
    m_queue.remove( it );
    result.detach();
    m_queueMutex.unlock();
    return result;
}

void Pipe::receiveMessage( ChannelMessage *msg, Channel::SendType )
{
    delete msg;
    emit dataAvailable();
}
