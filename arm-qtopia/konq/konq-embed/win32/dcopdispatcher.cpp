/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include <qglobal.h>
#if defined(Q_WS_WIN)
#include <windows.h>
#define GETHOSTNAMEDEFINED
#endif

#include "dcopobject.h"
#include "dcopdispatcher.h"
#include "dcopclient.h"
#include "kdebug.h"
#include "autolock.h"

#include <qsocketnotifier.h>

#include <kio/connection.h>

#include <assert.h>
#include <unistd.h>

// we use aggregation because making DCOPDispatcher inherit from
// Channel would mean more #ifdeffery in the header file
class DCOPDispatcherChannel : public Channel
{
public:
    DCOPDispatcherChannel( DCOPDispatcher *dispatcher );
protected:
    virtual void receiveMessage( ChannelMessage *message, Channel::SendType type );
private:
    DCOPDispatcher *m_dispatcher;
};

DCOPDispatcherChannel::DCOPDispatcherChannel( DCOPDispatcher *dispatcher )
    : Channel( dispatcher ), m_dispatcher( dispatcher )
{
}

void DCOPDispatcherChannel::receiveMessage( ChannelMessage *message, Channel::SendType type )
{
    m_dispatcher->receiveMessage( message, type );
}

// QThread::currentThread() is unusable on windows. :-(
#if defined(Q_WS_WIN)
#define K_CURRENTTHREAD GetCurrentThreadId
#else
#define K_CURRENTTHREAD QThread::currentThread
#endif

DCOPDispatcher *DCOPDispatcher::s_self = 0;

DCOPDispatcher::DCOPDispatcher()
    : m_currentRequest( 0 ), m_transactionAdded( false )
{
    // ### we rely on this being called in the gui thread. oh well.
    m_guiThread = K_CURRENTTHREAD();
    m_channel = new DCOPDispatcherChannel( this );
}

DCOPDispatcher::~DCOPDispatcher()
{
}

DCOPDispatcher *DCOPDispatcher::self()
{
    if ( !s_self )
        s_self = new DCOPDispatcher;
    return s_self;
}

bool DCOPDispatcher::process( Channel::SendType sendType, const QCString &appname,
                              const QCString &fun, const QByteArray &data,
                              QCString &replyType, QByteArray &replyData )
{
    if ( appname.isEmpty() )
        return false;

    // this is a bit tricky as we rely on the object still existing after
    // the unlock, but hey, we can't everything for free ;-)
    m_objpoolGuard.lock();
    DCOPObject *dco = m_objpool.find( appname );
    m_objpoolGuard.unlock();

    if ( !dco )
	return false;

    // if we are in the gui thread we can deliver right away.
    if ( K_CURRENTTHREAD() == m_guiThread )
        return dco->process(fun, data, replyType, replyData);
    
    // we are in a thread other than the gui thread. use postEvent and
    // a monitor.
    
    // the constructor takes care of detaching the data
    Request *request = new Request( appname, fun, data );

    m_channel->send( request, sendType );

    if ( sendType == Channel::OneWay )
	return true;

    replyType = request->replyType;
    replyData = request->replyData;
    bool res = request->result;
    delete request;
    return res;
}

void DCOPDispatcher::registerObject(DCOPObject *obj)
{
    AutoLock lock( m_objpoolGuard );
    m_objpool.insert( obj->objId(), obj );
}

void DCOPDispatcher::unregisterObject(DCOPObject *obj)
{
    AutoLock lock( m_objpoolGuard );
    m_objpool.remove( obj->objId() );
}

DCOPClientTransaction *DCOPDispatcher::beginTransaction()
{
    assert( m_currentRequest );
    assert( !m_transactionAdded );

    m_transactions.append( m_currentRequest );

    m_transactionAdded = true;

    return (DCOPClientTransaction *)m_currentRequest;
}

void DCOPDispatcher::endTransaction( DCOPClientTransaction *_transaction, QCString &replyType, QByteArray &replyData )
{
    Request *request = (Request *)_transaction;

    assert( m_transactions.findRef( request ) != -1 );

    m_transactions.removeRef( request );

    request->result = true;
    request->replyType = replyType;
    request->replyData = replyData;
    request->replyType.detach();
    request->replyData.detach();
    request->reply();
}

void DCOPDispatcher::receiveMessage( ChannelMessage *message, Channel::SendType type )
{
    Request *req = static_cast<Request *>( message );

    // transactions are only dealt with in the gui thread
    // -> no need for guards in this very case
    Request *oldCurrentRequest = m_currentRequest;
    bool oldTransactionAdded = m_transactionAdded;

    m_currentRequest = req;
    m_transactionAdded = false;
    
    bool res = process( type, req->appname, req->fun, req->data,
			req->replyType, req->replyData );

    if ( type == Channel::OneWay )
	delete req;
    else if ( !m_transactionAdded )
    {
	req->result = res;
	req->replyType.detach();
	req->replyData.detach();
	req->reply();
    }

    m_currentRequest = oldCurrentRequest;
    m_transactionAdded = oldTransactionAdded;
}

void DCOPDispatcher::slotDisconnectClient()
{
}

void DCOPDispatcher::slotDispatch( int )
{
}

#include "dcopdispatcher.moc"
