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

#include <assert.h>


DCOPObject::DCOPObject()
{
}

DCOPObject::DCOPObject( const QCString &name )
{
    setObjId( name );
}

DCOPObject::~DCOPObject()
{
    setObjId( QCString() ); // unregister
}

bool DCOPObject::process( const QCString &, const QByteArray &,
                          QCString &, QByteArray & )
{
    return false;
}

QCStringList DCOPObject::functions()
{
    return QCStringList();
}

QCStringList DCOPObject::interfaces()
{
    return QCStringList();
}

void DCOPObject::setObjId( const QCString &name )
{
    if ( !m_objId.isEmpty() )
        DCOPDispatcher::self()->unregisterObject( this );

    m_objId = name;

    if ( !m_objId.isEmpty() )
        DCOPDispatcher::self()->registerObject( this );
}

#include "dcopobject.moc"
