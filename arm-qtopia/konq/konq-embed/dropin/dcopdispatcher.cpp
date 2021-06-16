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

#include "dcopobject.h"
#include "dcopdispatcher.h"
#include "dcopclient.h"
#include "kdebug.h"

#include <qsocketnotifier.h>

#include <kio/connection.h>

#include <assert.h>
#include <unistd.h>

DCOPDispatcher::Client *DCOPDispatcher::s_currentClient = 0;
DCOPDispatcher *DCOPDispatcher::s_self = 0;

DCOPDispatcher::DCOPDispatcher()
{
    m_clients.setAutoDelete( true );
    m_transactions.setAutoDelete( true );
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

bool DCOPDispatcher::process( const QCString &appname,
                              const QCString &fun, const QByteArray &data,
                              QCString &replyType, QByteArray &replyData )
{
    if ( appname.isEmpty() )
        return false;

    DCOPObject *dco = m_objpool.find( appname );

    if( dco )
        return dco->process(fun, data, replyType, replyData);

    kdDebug() << "DCOPDispatcher: unknown service " << fun << endl;
    return false;
}

void DCOPDispatcher::registerObject(DCOPObject *obj)
{
    m_objpool.insert( obj->objId(), obj );
}

void DCOPDispatcher::unregisterObject(DCOPObject *obj)
{
    m_objpool.remove( obj->objId() );
}

DCOPClientTransaction *DCOPDispatcher::beginTransaction()
{
    assert( s_currentClient );
    assert( !m_transactionAdded );

    InternalDCOPClientTransaction *trans = new InternalDCOPClientTransaction;
    trans->m_client = s_currentClient;

    m_transactions.append( trans );

    m_transactionAdded = true;

    return (DCOPClientTransaction *)trans;
}

void DCOPDispatcher::endTransaction( DCOPClientTransaction *_transaction, QCString &replyType, QByteArray &replyData )
{
    InternalDCOPClientTransaction *transaction = (InternalDCOPClientTransaction *)_transaction;

    assert( m_transactions.findRef( transaction ) != -1 );

    QByteArray reply;
    QDataStream replyStream( reply, IO_WriteOnly );
    replyStream << replyType << replyData;

    transaction->m_client->m_connection->send( 0, reply );

    m_transactions.removeRef( transaction );
}

void DCOPDispatcher::addClient( KIO::Connection *connection, QObject *owner )
{
    // sanity check
    QListIterator<Client> it( m_clients );
    for (; it.current(); ++it )
        if ( it.current()->m_connection == connection ||
             it.current()->m_owner == owner )
            return; // this should not happen!

    Client *c = new Client;
    c->m_connection = connection;
    c->m_owner = owner;
    c->m_notifier = new QSocketNotifier( connection->fd_from(), QSocketNotifier::Read, owner );
    c->m_infd = connection->fd_from();
    m_clients.append( c );

    connect( c->m_owner, SIGNAL( destroyed() ),
             this, SLOT( slotDisconnectClient() ) );

    connect( c->m_notifier, SIGNAL( activated( int ) ),
             this, SLOT( slotDispatch( int ) ) );
}

void DCOPDispatcher::slotDisconnectClient()
{
    const QObject *owner = sender();
    disconnectClient( owner );
}

void DCOPDispatcher::disconnectClient( const QObject *owner )
{
    Client *c = m_clients.first();
    while ( c )
    {
        if ( c->m_owner == owner )
        {
            kdDebug() << "removed client!" << endl;
            int infd = c->m_connection->fd_from();
            int outfd = c->m_connection->fd_to();

            c->m_connection->close();

            delete c->m_connection;

            ::close( infd );
            ::close( outfd );

            // notifier is deleted by m_owner (as child obj)
            // hmm, appears to make trouble, better delete it ;)
            delete c->m_notifier;

            m_clients.remove();
            c = m_clients.current();
        }
        else
            c = m_clients.next();
    }
}

void DCOPDispatcher::slotDispatch( int sockfd )
{
    kdDebug() << "DCOPDispatcher::slotDispatch() " << sockfd << endl;

    // find client
    QListIterator<Client> it( m_clients );
    for (; it.current(); ++it )
        if ( it.current()->m_infd == sockfd )
        {
            dispatchClient( it.current() );
            return;
        }

    kdDebug() << "weird! received socket activation of unknown notifier!" << endl;
}

void DCOPDispatcher::dispatchClient( Client *client )
{
    int cmd;
    QByteArray data;
    int res = client->m_connection->read( &cmd, data );

    if ( res == -1 )
    {
        kdDebug() << "error reading from dcop client!" << endl;
        // ### FIXME
        disconnectClient( client->m_owner );
        return;
    }

    Client *oldClient = s_currentClient;
    s_currentClient = client;
    bool oldTransactionAdded = m_transactionAdded;
    m_transactionAdded = false;

    QDataStream stream( data, IO_ReadOnly );

    QCString remApp, remObj, remFun;
    QByteArray callData;

    stream >> remApp >> remObj >> remFun >> callData;

    QCString replyType;
    QByteArray replyData;

    // ### return value!
    process( remApp, remFun, callData, replyType, replyData );

    if ( cmd == DCOPClient::Call && !m_transactionAdded )
    {
        // send back reply

        QByteArray reply;
        QDataStream replyStream( reply, IO_WriteOnly );
        replyStream << replyType << replyData;

        client->m_connection->send( 0, reply );
    }

    s_currentClient = oldClient;
    m_transactionAdded = oldTransactionAdded;
}

#include "dcopdispatcher.moc"
