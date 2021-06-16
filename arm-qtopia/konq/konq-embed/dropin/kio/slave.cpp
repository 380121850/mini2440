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

#include "slave.h"

#include <kdebug.h>

using namespace KIO;

Slave::Slave( Connection *conn, pid_t pid )
    : SlaveInterface( conn )
{
    m_pid = pid;
    conn->connect( this, SLOT( slotDispatch() ) );
}

Slave::~Slave()
{
    kdDebug() << "Slave::~Slave()" << endl;
#if !defined(Q_WS_WIN)
    if ( connection()->inited() ) {
        int infd = connection()->fd_from();
        int outfd = connection()->fd_to();
        connection()->close();
        ::close( infd );
        ::close( outfd );
    }   
#endif

    // ###
    delete connection();
}

void Slave::slotDispatch()
{
    //    kdDebug() << "Slave::slotDispatch()" << endl;
    if ( !dispatch() )
    {
	qDebug( "Slave::slotDispatch() : abort!" );
        connection()->close();

        // ### FIXME!
//        emit slaveDied( this ); // this will make the scheduler delete us
    }
}

void Slave::kill()
{
    kdDebug() << "Slave::kill()" << endl;
    qDebug( "Slave::kill()" );
#if defined(Q_WS_WIN)
    connection()->sendnow( CMD_DISCONNECT );
#else
    if ( m_pid )
        ::kill( m_pid, SIGTERM );
#endif
}

void Slave::setConfig( const MetaData &config )
{
    QByteArray data;
    QDataStream stream( data, IO_WriteOnly );
    stream << config;
    send( CMD_CONFIG, data );
}

#include "slave.moc"
