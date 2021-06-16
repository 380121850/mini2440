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

#include "bookmarkeditorimpl.h"

#if defined(ENABLE_BOOKMARKS)

#include "bookmarkeditimpl.h"

#include <config.h>

#include "bookmarks.h"
#include "xmltree.h"

#include <klocale.h>

#include <qpushbutton.h>
#include <qtoolbutton.h>

#include <assert.h>

BookmarkItem::BookmarkItem( QListView *parent, QListViewItem *after, const Bookmark &bookmark )
    : QListViewItem( parent, after, bookmark.title(), bookmark.url() ), m_bookmark( bookmark )
{
}

void BookmarkItem::update()
{
    setText( 0, m_bookmark.title() );
    setText( 1, m_bookmark.url() );
}

BookmarkEditor::BookmarkEditor( XMLElement *bookmarks, QWidget *parent, const char *name )
    : BookmarkEditorBase( parent, name, true ), m_bookmarks( bookmarks )
{
#if defined(KONQ_GUI_QPE)
    okButton->hide();
    cancelButton->hide();
#endif

    view->setSorting( -1 );

    refill();

#if defined(_WS_QWS_)
    showMaximized();
#endif
}

BookmarkEditor::~BookmarkEditor()
{
    delete m_bookmarks;
}

void BookmarkEditor::deleteBookmark()
{
    BookmarkItem *it = static_cast<BookmarkItem *>( view->selectedItem() );
    XMLElement *e = it->bookmark().xmlElement();
    e->parent()->removeChild( e );
    delete e;
    delete it;
}

void BookmarkEditor::moveBookmarkDown()
{
    BookmarkItem *it = static_cast<BookmarkItem *>( view->selectedItem() );
    XMLElement *e = it->bookmark().xmlElement();

    // find the right XMLElement below (it is not necessarily nextChild()
    // but can be lower when having folders

    XMLElement *nextElement = e->nextChild();
    BookmarkItem *nextItem = static_cast<BookmarkItem *>( it->itemBelow() );

    while ( nextElement && nextElement != nextItem->bookmark().xmlElement() )
        nextElement = nextElement->nextChild();

    // let's see if this happens :)
    assert( nextElement );

    e->parent()->insertAfter( e, nextElement );

    it->moveItem( nextItem );
    updateButtons();
}

void BookmarkEditor::moveBookmarkUp()
{
    BookmarkItem *it = static_cast<BookmarkItem *>( view->selectedItem() );
    XMLElement *e = it->bookmark().xmlElement();

    // find the right XMLElement above (it is not necessarily prevChild()
    // but can be higher when having folders
    XMLElement *prevElement = e->prevChild();
    BookmarkItem *prevItem = static_cast<BookmarkItem *>( it->itemAbove() );

    while ( prevElement && prevElement != prevItem->bookmark().xmlElement() )
        prevElement = prevElement->prevChild();

    // let's see if this happens :)
    assert( prevElement );

    e->parent()->insertBefore( e, prevElement );

    if ( !prevItem->itemAbove() )
    {
        view->takeItem( it );
        view->insertItem( it );
        view->setSelected( it, true );
    }
    else
        it->moveItem( prevItem->itemAbove() );
    updateButtons();
}

void BookmarkEditor::newBookmark()
{
    BookmarkEdit *edit = new BookmarkEdit( this );
    edit->setCaption( i18n( "New Bookmark" ) );
    if ( edit->exec() == QDialog::Accepted )
    {
        XMLElement *elem = new XMLElement;
        elem->setTagName( "bookmark" );
        m_bookmarks->firstChild()->appendChild( elem );
 
        Bookmark bm( elem );
        edit->saveTo( bm );

        QListViewItem *last = view->firstChild();
        while ( last->itemBelow() )
            last = last->itemBelow();

        BookmarkItem *it = new BookmarkItem( view, last, bm ); 
        view->setSelected( it, true );
        view->ensureItemVisible( it );
        updateButtons();
    }
    delete edit;
}

void BookmarkEditor::editBookmark()
{
    BookmarkItem *it = static_cast<BookmarkItem *>( view->selectedItem() );

    BookmarkEdit *edit = new BookmarkEdit( it->bookmark(), this );
    edit->setCaption( i18n( "Edit Bookmark" ) );
    if ( edit->exec() == QDialog::Accepted )
    {
        edit->saveTo( it->bookmark() );
        it->update();
    }
    delete edit;
}

void BookmarkEditor::updateButtons()
{
    QListViewItem *it = view->selectedItem();

    bool haveItem = it != 0;
    bool firstItem = haveItem ? it->itemAbove() != 0 : false;
    bool lastItem = haveItem ? it->itemBelow() != 0 : false;

    moveUpButton->setEnabled( firstItem );
    moveDownButton->setEnabled( lastItem );
    deleteBookmarkButton->setEnabled( haveItem );
    editBookmarkButton->setEnabled( haveItem );
}

void BookmarkEditor::refill()
{
    view->clear();

    BookmarkFolder root( m_bookmarks->firstChild() );
    if ( !root.isFolder() )
        return;

    QListViewItem *last = 0;
    Bookmark bm = root.first();
    for (; !bm.isNull(); bm = root.next( bm ) )
    {
        if ( bm.isFolder() )
            continue;
   
        last = new BookmarkItem( view, last, bm );
    }

    updateButtons();
}

#include "bookmarkeditorimpl.moc"

#endif // ENABLE_BOOKMARKS

/*
 * vim:et
 */
