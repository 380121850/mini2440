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

*/
#ifndef __launcher_h__
#define __launcher_h__

struct cmsghdr;

#include <qstring.h>

#include <sys/types.h>
#include <unistd.h>

namespace KIO
{
    class Slave;

    class Launcher
    {
    public:
        static void startLauncherProcess();

        void stopLauncherProcess();

        static Launcher *self();

        Slave *createSlave( const QString &prot );

    private:
#if !defined(Q_WS_WIN)
        struct LauncherMsg
        {
            unsigned char command;

            union
            {
                pid_t slavePid;
                char protocol[ 16 ];
            } data;
        };

        Launcher( int socket );
        Launcher( int socket, pid_t launcherPid );
        ~Launcher();

        pid_t sendCreateSlaveCommand( const char *protocol,
                                      int slaveReadFd, int slaveWriteFd,
                                      int dcopReadFd, int dcopWriteFd );

        void dispatchLoop();

        bool dispatch( LauncherMsg &msg, cmsghdr *controlmsg );

        bool createSlaveInternal( const char *protocol, cmsghdr *controlmsg );

        static const unsigned char slaveFdCount = 4;

        static const unsigned char launcherCreateSlaveCmd = 1;
        static const unsigned char launcherSlavePidCmd = 2;

        int m_socket;
        pid_t m_launcherPid;
#endif

        static Launcher *s_self;
    };

};

#endif // __launcher_h__
