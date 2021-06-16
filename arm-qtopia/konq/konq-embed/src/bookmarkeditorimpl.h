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


#ifndef __bookmarkeditorimpl_h__
#define __bookmarkeditorimpl_h__

#include <config.h>

#if defined(ENABLE_BOOKMARKS)

#include "bookmarkeditor.h"

#include "bookmarks.h"

class XMLElement;

#include <qlistview.h>

class BookmarkItem : public QListViewItem
{
public:
    BookmarkItem( QListView *parent, QListViewItem *after, const Bookmark &bookmark );

    Bookmark bookmark() const { return m_bookmark; }

    void update();

private:
    Bookmark m_bookmark;
};

class BookmarkEditor : public BookmarkEditorBase
{
    Q_OBJECT
public:
    // note: ownership of 'bookmarks' is transferred!
    BookmarkEditor( XMLElement *bookmarks, QWidget *parent, const char *name = 0 );
    virtual ~BookmarkEditor();

    XMLElement *donateBookmarks() 
    { XMLElement *res = m_bookmarks; m_bookmarks = 0; return res; }

protected slots:
    virtual void deleteBookmark();
    virtual void moveBookmarkDown();
    virtual void moveBookmarkUp();
    virtual void newBookmark();
    virtual void editBookmark();
    virtual void updateButtons();

private:
    void refill();

    XMLElement *m_bookmarks;
};

#endif // ENABLE_BOOKMARKS

#endif

/*
 * vim:et
 */

