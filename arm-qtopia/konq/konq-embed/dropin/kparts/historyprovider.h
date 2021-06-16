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

// Modified by Simon Hausmann <simon@kde.org> :
// Made it a almost-noop class for Konqueror/Embedded

#ifndef KHISTORYPROVIDER_H
#define KHISTORYPROVIDER_H

#include <qobject.h>

namespace KParts {

/**
 * Basic class to manage a history of "items". This class is only meant
 * for fast lookup, if an item is in the history or not.
 *
 * May be subclassed to implement a persistent history for example.
 */
class HistoryProvider : public QObject
{
public:
    static HistoryProvider * self();

    /**
     * Creates a KHistoryProvider with an optional parent and name
     */
    HistoryProvider() {}

    /**
     * Destroys the provider.
     */
    ~HistoryProvider() {}

    /**
     * @returns true if @p item is present in the history.
     */
    bool contains( const QString& ) const { return false; }

    /**
     * Inserts @p item into the history.
     */
    void insert( const QString& ) {}

    /**
     * Removes @p item from the history.
     */
    void remove( const QString& ) {}

    /**
     * Clears the history.
     */
    void clear() {}

private:
    static HistoryProvider *s_self;
};

};

#endif // KHISTORYPROVIDER_H
