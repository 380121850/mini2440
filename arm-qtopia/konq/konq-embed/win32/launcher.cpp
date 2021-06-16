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
#include "resolver.h"

#include <assert.h>

#include <kdebug.h>
#include <kio/connection.h>
#include <kio/slave.h>
#include <kio/slavebase.h>
#include <dcopdispatcher.h>
#include <qthread.h>
#include <pipe.h>

using namespace KIO;

class ThreadKiller : public QObject
{
public:
    static void initialize();
    static void deleteThread( QThread *thr );
protected:
    virtual bool event( QEvent *ev );
private:
    static ThreadKiller *s_self;
};

ThreadKiller *ThreadKiller::s_self = 0;

void ThreadKiller::initialize()
{
    assert( !s_self );
    s_self = new ThreadKiller;
}

void ThreadKiller::deleteThread( QThread *thr )
{
    assert( s_self );
    QThread::postEvent( s_self, new QCustomEvent( static_cast<QEvent::Type>( QEvent::User + 41 ),
						  thr ) );
}

bool ThreadKiller::event( QEvent *ev )
{
    if ( ev->type() == static_cast<QEvent::Type>( QEvent::User + 41 ) )
    {
	//we are in the main thread. let's delete the thread object we
	//were asked to delete.
	QCustomEvent *ce = static_cast<QCustomEvent *>( ev );
	QThread *thr = static_cast<QThread *>( ce->data() );
	thr->wait();
	delete thr;
	return true;
    }
    return QObject::event( ev );
}

class SlaveThread : public QThread
{
public:
    SlaveThread( SlaveBase *slave, Pipe *slave2AppPipe, Pipe *app2SlavePipe )
	: m_slave( slave ), m_slave2AppPipe( slave2AppPipe ), m_app2SlavePipe( app2SlavePipe )
    {
    }

    virtual ~SlaveThread()
    {
	delete m_slave2AppPipe;
	delete m_app2SlavePipe;
    }

    virtual void run()
    {
	m_slave->dispatchLoop();
       	ThreadKiller::deleteThread( this );
    }
private:
    SlaveBase *m_slave;
    Pipe *m_slave2AppPipe;
    Pipe *m_app2SlavePipe;
};

Launcher *Launcher::s_self = 0;

Launcher *Launcher::self()
{
    if ( !s_self )
        startLauncherProcess();

    return s_self;
}

KIO::Slave *Launcher::createSlave( const QString &prot )
{
    SlaveBase *slaveBase = SlaveBase::createSlave( prot );
    if ( !slaveBase )
	return 0;

    Pipe *thread2App = new Pipe;
    Pipe *app2Thread = new Pipe;

    // we pass the ownership of the two pipes to SlaveThread as
    // that one dies at the very last (Slave() dies before) , so it
    // sounds save to me to delete the pipes in the SlaveThread dtor then.
    SlaveThread *thr = new SlaveThread( slaveBase, thread2App, app2Thread );

    KIO::Connection *threadConn = new KIO::Connection( app2Thread, thread2App );
    KIO::Connection *appConn = new KIO::Connection( thread2App, app2Thread );

    slaveBase->setConnection( threadConn );
    Slave *slave = new Slave( appConn, 0 );

    thr->start();
    return slave;
}

void Launcher::startLauncherProcess()
{
    s_self = new Launcher();
    ThreadKiller::initialize();
    Resolver::initStatic();
}

void Launcher::stopLauncherProcess()
{
    delete this;
    s_self = 0;
}



