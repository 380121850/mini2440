/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

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

#include "launcher.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <kdebug.h>
#include <kio/connection.h>
#include <kio/slave.h>
#include <kio/slavebase.h>
#include <dcopdispatcher.h>

extern void setupSignalHandler();

using namespace KIO;

Launcher *Launcher::s_self = 0;

Launcher *Launcher::self()
{
    if ( !s_self )
        startLauncherProcess();

    return s_self;
}

KIO::Slave *Launcher::createSlave( const QString &prot )
{
    int app2slave[ 2 ]; // app -> slave
    int slave2app[ 2 ]; // slave -> app
    int dcopApp2Slave[ 2 ]; // dcop: app -> slave
    int dcopSlave2App[ 2 ]; // dcop: slave -> app

    if ( pipe( app2slave ) == -1 )
    {
        perror( "FATAL: can't create pipe for app->slave communication" );
        ::exit( 0 );
    }

    if ( pipe( slave2app ) == -1 )
    {
        perror( "FATAL: can't create pipe for slave->app communcation" );
        ::exit( 0 );
    }

    if ( pipe( dcopApp2Slave ) == -1 )
    {
        perror( "FATAL: can't create pipe for dcop app->slave communcation" );
        ::exit( 0 );
    }

    if ( pipe( dcopSlave2App ) == -1 )
    {
        perror( "FATAL: can't create pipe for dcop slave->app communication" );
        ::exit( 0 );
    }

    pid_t slavePid = sendCreateSlaveCommand( prot.latin1(),
                                             app2slave[ 0 ], slave2app[ 1 ],
                                             dcopApp2Slave[ 0 ], dcopSlave2App[ 1 ] );

    // we can close these, now that they have been transferred to the slave
    ::close( app2slave[ 0 ] );
    ::close( slave2app[ 1 ] );
    ::close( dcopApp2Slave[ 0 ] );
    ::close( dcopSlave2App[ 1 ] );

    if ( slavePid == 0 )
    {
        ::close( app2slave[ 1 ] );
        ::close( slave2app[ 0 ] );
        ::close( dcopApp2Slave[ 1 ] );
        ::close( dcopSlave2App[ 0 ] );

        return 0;
    }

    Connection *connection = new Connection;
    connection->init( slave2app[ 0 ], app2slave[ 1 ] );

    Slave *slave = new Slave( connection, slavePid );

    connection = new Connection;
    connection->init( dcopSlave2App[ 0 ], dcopApp2Slave[ 1 ] );

    DCOPDispatcher::self()->addClient( connection, slave );

    return slave;
}

Launcher::Launcher( int socket )
    : m_socket( socket ), m_launcherPid( 0 )
{
}

Launcher::Launcher( int socket, pid_t launcherPid )
    : m_socket( socket ), m_launcherPid( launcherPid )
{
}

Launcher::~Launcher()
{
    if ( m_launcherPid != 0 )
        ::kill( m_launcherPid, SIGTERM );
}

void Launcher::startLauncherProcess()
{
    int launcherFds[ 2 ];

    if ( socketpair( AF_UNIX, SOCK_STREAM, 0, launcherFds ) != 0 )
    {
        perror( "FATAL: can't create socket for launcher" );
        ::exit( 1 );
    }

    pid_t pid = fork();

    if ( pid == 0 )
    {
        ::close( launcherFds[ 0 ] );

        Launcher l( launcherFds[ 1 ] );
        l.dispatchLoop();

        ::exit( 0 );
    }

    ::close( launcherFds[ 1 ] );

    s_self = new Launcher( launcherFds[ 0 ], pid );
}

void Launcher::stopLauncherProcess()
{
    delete this;
    s_self = 0;
}

pid_t Launcher::sendCreateSlaveCommand( const char *protocol,
                                        int slaveReadFd, int slaveWriteFd,
                                        int dcopReadFd, int dcopWriteFd )
{
    LauncherMsg launcherMsg;

    iovec iov;

    // size of hdr plus size of four fds
    // (kio: app->slave and slave->app)
    // (dcop: app->slave and slave->app)
    size_t hdrlen = CMSG_LEN( sizeof( int ) * slaveFdCount );
    cmsghdr *hdr = (cmsghdr *)malloc( hdrlen );

    msghdr msg;

    iov.iov_base = (char *)&launcherMsg;
    iov.iov_len = sizeof( LauncherMsg );

    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = (char *)hdr;
    msg.msg_controllen = hdrlen;
    msg.msg_flags = 0;

    hdr->cmsg_len = hdrlen;
    hdr->cmsg_level = SOL_SOCKET;
    hdr->cmsg_type = SCM_RIGHTS;

    int *fdPtr = reinterpret_cast<int *>( CMSG_DATA( hdr ) );
    *fdPtr++ = slaveReadFd;
    *fdPtr++ = slaveWriteFd;
    *fdPtr++ = dcopReadFd;
    *fdPtr = dcopWriteFd;

    memset( &launcherMsg, 0, sizeof( launcherMsg ) );
    launcherMsg.command = launcherCreateSlaveCmd;
    strncpy( launcherMsg.data.protocol, protocol, sizeof( launcherMsg.data.protocol ) );
    launcherMsg.data.protocol[ sizeof( launcherMsg.data.protocol ) - 1 ] = 0;

    if ( sendmsg( m_socket, &msg, 0 ) == -1 )
    {
        perror( "error sending launcherCreateSlaveCmd" );
        free( hdr );
        return 0;
    }

    msg.msg_name = 0;
    msg.msg_namelen = 0;

    int n = recvmsg( m_socket, &msg, MSG_WAITALL );

    free( hdr );

    if ( n == -1 )
        return 0;

    if ( launcherMsg.command != launcherSlavePidCmd )
    {
        kdDebug() << "received unexpected command " << launcherMsg.command << " from launcher!" << endl;
        return 0;
    }

    return launcherMsg.data.slavePid;
}

void Launcher::dispatchLoop()
{
    qDebug( "Launcher::dispatchLoop()..." );

    setupSignalHandler();
    while ( 42 )
    {
        LauncherMsg launcherMsg;

        iovec iov;

        // size of hdr plus size of four fds
        // (kio: app->slave and slave->app)
        // (dcop: app->slave and slave->app)
        size_t hdrlen = CMSG_LEN( sizeof( int ) * slaveFdCount );
        cmsghdr *hdr = (cmsghdr *)malloc( hdrlen );

        msghdr msg;

        iov.iov_base = (char *)&launcherMsg;
        iov.iov_len = sizeof( launcherMsg );

        msg.msg_name = 0;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = (char *)hdr;
        msg.msg_controllen = hdrlen;
        msg.msg_flags = 0;

        if ( recvmsg( m_socket, &msg, MSG_WAITALL ) == -1 )
        {
            perror( "launcher: recvmsg() failed" );
            free( hdr );
            return;
        }

        bool r = dispatch( launcherMsg, hdr );

        free( hdr );

        // something went wrong :-(
        if ( !r )
            break;
    }
}

bool Launcher::dispatch( LauncherMsg &msg, cmsghdr *controlmsg )
{
    if ( msg.command == launcherCreateSlaveCmd )
        return createSlaveInternal( msg.data.protocol, controlmsg );
    else {
        kdDebug() << "launcher: received unknown command " << msg.command << endl;
        return false;
    }
}

bool Launcher::createSlaveInternal( const char *protocol, cmsghdr *controlmsg )
{
    qDebug( "launcher: creating slave for protocol %s", protocol );

    int *fdPtr = reinterpret_cast<int *>( CMSG_DATA( controlmsg ) );

    int slaveReadFd = *fdPtr++;
    int slaveWriteFd = *fdPtr++;
    int dcopReadFd = *fdPtr++;
    int dcopWriteFd = *fdPtr;

    pid_t pid = fork();

    if ( pid == 0 )
    {
        // here we are, in the slave, finally :-)

        Connection *connection = new Connection;
        connection->init( slaveReadFd, slaveWriteFd );

        SlaveBase *slave = SlaveBase::createSlave( QString::fromLatin1( protocol ) );

        assert( slave );

        slave->setConnection( connection );

        connection = new Connection;
        connection->init( dcopReadFd, dcopWriteFd );

        DCOPClient::setGlobalConnection( connection );

        slave->dispatchLoop();

        ::exit( 0 );
    }

    assert( pid != -1 );

    ::close( slaveReadFd );
    ::close( slaveWriteFd );
    ::close( dcopReadFd );
    ::close( dcopWriteFd );

    // send back pid to main
    LauncherMsg launcherMsg;

    iovec iov;

    msghdr msg;

    iov.iov_base = (char *)&launcherMsg;
    iov.iov_len = sizeof( launcherMsg );

    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = 0;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    memset( &launcherMsg, 0, sizeof( launcherMsg ) );
    launcherMsg.command = launcherSlavePidCmd;
    launcherMsg.data.slavePid = pid;

    if ( sendmsg( m_socket, &msg, 0 ) == -1 ) {
        perror( "launcher: error sending launcherSlavePidCmd" );
        return false;
    }

    return true;
}

