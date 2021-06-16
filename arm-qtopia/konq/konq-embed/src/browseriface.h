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


#ifndef __browseriface_h__
#define __browseriface_h__

#include <kparts/browserinterface.h>

class BrowserView;

class BrowserInterface : public KParts::BrowserInterface
{
    Q_OBJECT
    Q_PROPERTY( uint historyLength READ historyLength );
public:
    BrowserInterface( BrowserView *view, const char *name );

    uint historyLength() const;

#ifdef QT_NO_PROPERTIES
    virtual QVariant property( const char *name ) const;
#endif

public slots:
    void goHistory( int );

private:
    BrowserView *m_view;
};

#endif
