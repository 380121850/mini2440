/* This file is part of the KDE libraries
    Copyright (c) 1997,2001 Stephan Kulow <coolo@kde.org>
    Copyright (c) 1999 Preston Brown <pbrown@kde.org>
    Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 
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

#include "klocale.h"

QString put_n_in(const QString &orig, unsigned long n)
{
    QString ret = orig;
    int index = ret.find("%n");
    if (index == -1)
        return ret;
    ret.replace(index, 2, QString::number(n));
    return ret;
}

// ### hack
QString i18n( const char *singular, const char *plural, unsigned long n )                                
{
    if (n == 1)
        return put_n_in(QObject::tr(singular), n);
    else
        return put_n_in(QObject::tr(plural), n);
}

/*
 * vim:et
 */
