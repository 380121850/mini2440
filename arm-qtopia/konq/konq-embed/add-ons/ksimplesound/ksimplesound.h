/* This file is part of the KDE Project
   Copyright (c) 2002 Paul Chitescu <paulc-devel@null.ro>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KSIMPLESOUND_H
#define __KSIMPLESOUND_H

#include <config.h>
#include <qobject.h>

#ifdef QT_NO_SOUND
#error KSimpleSound needs QSound support compiled in Qt
#endif

#include <kparts/part.h>
#include <kparts/factory.h>

class KSimpleSound : public KParts::ReadOnlyPart
{
	Q_OBJECT
public:
    KSimpleSound( QWidget *parentWidget, const char *widgetName,
		  QObject *parent, const char *name,
		  const QStringList &args = QStringList() );

    virtual ~KSimpleSound();

public slots:
    virtual void play();

protected slots:
    void fileOpen();

protected:
    virtual bool openFile();
    bool m_autoplay;
    bool m_loop;
};

class SimpleSoundFactory : public KParts::Factory
{
	Q_OBJECT
public:
    SimpleSoundFactory();
    virtual ~SimpleSoundFactory();

    static KInstance* instance();

protected:
    virtual KParts::Part* createPartObject( QWidget *parentWidget, const char *widgetName,
                                            QObject *parent, const char *name,
					    const char *classname = "KParts::ReadOnlyPart",
					    const QStringList &args = QStringList() );
};

#endif // __KSIMPLESOUND_H
