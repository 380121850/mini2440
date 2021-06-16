#include <config.h>
#if defined(ENABLE_BOOKMARKS)
/****************************************************************************
** Form interface generated from reading ui file '../../../../src/kdenox/konq-embed/src/bookmarkeditor.ui'
**
** Created: Mon Mar 11 10:36:21 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef BOOKMARKEDITORBASE_H
#define BOOKMARKEDITORBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QListView;
class QListViewItem;
class QPushButton;
class QToolButton;

class BookmarkEditorBase : public QDialog
{ 
    Q_OBJECT

public:
    BookmarkEditorBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~BookmarkEditorBase();

    QFrame* Frame4;
    QToolButton* newBookmarkButton;
    QToolButton* editBookmarkButton;
    QToolButton* deleteBookmarkButton;
    QFrame* Line1;
    QToolButton* moveUpButton;
    QToolButton* moveDownButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QListView* view;

protected slots:
    virtual void deleteBookmark();
    virtual void editBookmark();
    virtual void moveBookmarkDown();
    virtual void moveBookmarkUp();
    virtual void newBookmark();
    virtual void updateButtons();

protected:
    QGridLayout* BookmarkEditorBaseLayout;
    QHBoxLayout* Frame4Layout;
};

#endif // BOOKMARKEDITORBASE_H
#endif
