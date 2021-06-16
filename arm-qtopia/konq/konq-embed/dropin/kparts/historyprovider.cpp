/* This file is part of the KDE project
   Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>

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

#include "historyprovider.h"

// Modified by Simon Hausmann <simon@kde.org> :
// Made it a almost-noop class for Konqueror/Embedded

using namespace KParts;

HistoryProvider * HistoryProvider::s_self = 0L;

HistoryProvider * HistoryProvider::self()
{
    if ( !s_self )
        // ### Simon: I introduced a leak. Maybe make it inherit from QObject
        // again and provide qApp as parent (like it is done in the original
        // src) ?
	s_self = new HistoryProvider;
    return s_self;
}

