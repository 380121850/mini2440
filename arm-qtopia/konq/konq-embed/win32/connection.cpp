/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>
		       Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id: connection.cpp,v 1.1.2.1 2001/10/11 12:36:22 hausmann Exp $

#include <qtimer.h>
#include <qapplication.h>

#include "connection.h"

#include <kdebug.h>
#include <pipe.h>

using namespace KIO;

template class QList<KIO::Task>;

Connection::Connection( Pipe *inPipe, Pipe *outPipe )
{
    m_inPipe = inPipe;
    m_outPipe = outPipe;
    receiver = 0;
    member = 0;
    queueonly = false;
    m_suspended = false;
    unqueuedTasks = 0;
}

Connection::~Connection()
{
    //    close();
}

void Connection::suspend()
{
    m_suspended = true;
    
    // ### is that lock necessary?
    qApp->lock();
    m_inPipe->blockSignals( true );
    qApp->unlock();
}

void Connection::resume()
{
    m_suspended = false;
    qApp->lock();
    m_inPipe->blockSignals( false );
    qApp->unlock();
}

void Connection::close()
{
    // ###
}

void Connection::send(int cmd, const QByteArray& data)
{
    if (queueonly || tasks.count() > 0) {
	Task *task = new Task();
	task->cmd = cmd;
	task->data = data;
	tasks.append(task);
    } else {
	sendnow( cmd, data );
    }

    if (tasks.count() > 0 && !queueonly)
	QTimer::singleShot(100, this, SLOT(dequeue()));
}

void Connection::queueOnly(bool queue) {
    unqueuedTasks = tasks.count();
    queueonly = queue;
    dequeue();
}

void Connection::dequeue()
{
    if (tasks.count() == 0  || (queueonly && unqueuedTasks == 0))
	return;

    tasks.first();
    Task *task = tasks.take();
    sendnow( task->cmd, task->data );
    delete task;

    if (queueonly)
	unqueuedTasks--;

    if (tasks.count() > 0 && (!queueonly || unqueuedTasks > 0))
	QTimer::singleShot(100, this, SLOT(dequeue()));
}

void Connection::connect(QObject *_receiver, const char *_member)
{
    receiver = _receiver;
    member = _member;
    if (receiver) {
        if ( m_suspended )
            suspend();
	QObject::connect(m_inPipe, SIGNAL(dataAvailable()), receiver, member);
    }
}

bool Connection::sendnow( int _cmd, const QByteArray &data )
{
    QByteArray outData;
    QDataStream stream( outData, IO_WriteOnly );
    stream << _cmd << data;
    m_outPipe->write( outData );
    return true;
}

int Connection::read( int* _cmd, QByteArray &data )
{
    QByteArray inData = m_inPipe->read();
    QDataStream stream( inData, IO_ReadOnly );
    stream >> *_cmd >> data;
    return data.size();
}

#include "connection.moc"
