// -*- c++ -*-
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

#ifndef __connection_h__
#define __connection_h__ "$Id: connection.h,v 1.1.2.1 2001/10/11 12:36:22 hausmann Exp $"

#include <qcstring.h>
#include <qobject.h>

class Pipe;

namespace KIO {

    struct Task {
	int cmd;
	QByteArray data;
    };

    /**
     * This class provides a simple means for IPC between two applications
     * via a pipe.
     * It handles a queue of commands to be sent, and has a internal signal
     * called after a command has been sent, to send the next one (FIFO).
     */
    class Connection : public QObject
    {
	Q_OBJECT
    public:
	Connection( Pipe *inPipe, Pipe *outPipe );
	virtual ~Connection();

	int fd_from() const { return -1; }
	int fd_to() const { return -1; }

	void connect(QObject *receiver = 0, const char *member = 0);
	void close();
	
	// send (queues the command to be sent)
	void send(int cmd, const QByteArray &arr = QByteArray());

	// send (without queue)
	bool sendnow( int _cmd, const QByteArray &data = QByteArray() );

	/**
	 * Receive data
	 *
	 * @return >=0 indicates the received data size upon success
	 *         -1  indicates error
	 */
	int read( int* _cmd, QByteArray & );

        /**
         * Don't handle incoming data until resumed
         */
        void suspend();

        /**
         * Resume handling of incoming data
         */
        void resume();

        /**
         * Returns status of connection
         */
        bool suspended() const { return m_suspended; }

	void queueOnly(bool queue);

    protected slots:
        void dequeue();
	
    protected:
	
	
    private:
	bool queueonly;
	Pipe *m_inPipe;
	Pipe *m_outPipe;
	QObject *receiver;
	const char *member;
	QList<Task> tasks;
	int unqueuedTasks;
        bool m_suspended;
    };

};

#endif
