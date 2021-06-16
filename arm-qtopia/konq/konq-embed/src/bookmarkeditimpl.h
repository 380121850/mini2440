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


#ifndef __bookmarkeditimpl_h__
#define __bookmarkeditimpl_h__

#include <config.h>

#if defined(ENABLE_BOOKMARKS)

#include "bookmarkedit.h"
#include "bookmarks.h"

class BookmarkEdit : public BookmarkEditBase
{
public:
    BookmarkEdit( const Bookmark &bookmark,
                  QWidget *parent, const char *name = 0 );
    BookmarkEdit( QWidget *parent, const char *name = 0 );

    void saveTo( Bookmark bookmark );

private:
    void init();
};

#endif // ENABLE_BOOKMARKS

#endif
/*
 * vim:et
 */
