/*
    This file is part of the KDE libraries

    Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)

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
//----------------------------------------------------------------------------
//
// KDE HTML Widget -- String class
// $Id: stringit.h,v 1.11 2001/06/10 15:22:32 mueller Exp $

#ifndef KHTMLSTRING_H
#define KHTMLSTRING_H

#include "dom_string.h"
using namespace DOM;

#include <qstring.h>

namespace khtml
{

class DOMStringIt
{
public:
    DOMStringIt()
	{ s = 0, l = 0; }
    DOMStringIt(QChar *str, uint len)
	{ s = str, l = len; }
    DOMStringIt(const QString &str)
	{ s = str.unicode(); l = str.length(); }
    DOMStringIt(const DOMString &str)
	{ s = str.unicode(); l = str.length(); }

    DOMStringIt *operator++()
    {
        if(!pushedChar.isNull())
            pushedChar=0;
        else if(l > 0 )
	    s++, l--;
	return this;
    }
public:
    void push(const QChar& c) { /* assert(pushedChar.isNull());*/  pushedChar = c; }
    
    const QChar& operator*() const  { return pushedChar.isNull() ? *s : pushedChar; }
    const QChar* operator->() const { return pushedChar.isNull() ? s : &pushedChar; }

    bool escaped() const { return !pushedChar.isNull(); }
    uint length() const { return l+(!pushedChar.isNull()); }

    const QChar *current() const { return pushedChar.isNull() ? s : &pushedChar; }

protected:
    QChar pushedChar;
    const QChar *s;
    int l;
};


};

#endif
