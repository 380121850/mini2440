#ifndef __pipe_h__
#define __pipe_h__

#include <qobject.h>
#include <qthread.h>
#include <channel.h>

class Pipe : public Channel
{
    Q_OBJECT
public:
    Pipe( QObject *parent = 0, const char *name = 0 );
    virtual ~Pipe();

    void write( const QByteArray &input );
    QByteArray read();

signals:
    void dataAvailable();

protected:
    virtual void receiveMessage( ChannelMessage *, Channel::SendType type );

private:
    QMutex m_queueMutex;
    QValueList<QByteArray> m_queue;
    QWaitCondition m_readCondition;
};

#endif
