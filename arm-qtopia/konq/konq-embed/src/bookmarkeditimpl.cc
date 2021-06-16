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

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/


#include "bookmarkeditimpl.h"

#if defined(ENABLE_BOOKMARKS)

#include "mainwindowbase.h"

#include <qlineedit.h>
#include <qpushbutton.h>

BookmarkEdit::BookmarkEdit( const Bookmark &bookmark,
                            QWidget *parent, const char *name )
    : BookmarkEditBase( parent, name, true )
{
    title->setText( bookmark.title() ); 
    location->setText( bookmark.url() );

    init();
}

BookmarkEdit::BookmarkEdit( QWidget *parent, const char *name )
    : BookmarkEditBase( parent, name, true )
{
#if defined(KONQ_GUI_QPE)
    location->setText( "http://www." );
#endif

    init();
}

void BookmarkEdit::saveTo( Bookmark bookmark )
{
    bookmark.setTitle( title->text() );
    bookmark.setURL( location->text() );
}

void BookmarkEdit::init()
{
#if defined(KONQ_GUI_QPE)
    okButton->hide();
    cancelButton->hide();
#endif
#if defined(_WS_QWS_)
    if ( MainWindowBase::STYLE_WINDOWED != MainWindowBase::s_winStyle )
	showMaximized();
#endif
}

#endif // ENABLE_BOOKMARKS

