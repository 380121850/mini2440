#include <config.h>
#if defined(ENABLE_BOOKMARKS)
/****************************************************************************
** Form interface generated from reading ui file '../../../../src/kdenox/konq-embed/src/bookmarkedit.ui'
**
** Created: Mon Mar 11 10:36:22 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef BOOKMARKEDITBASE_H
#define BOOKMARKEDITBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QPushButton;

class BookmarkEditBase : public QDialog
{ 
    Q_OBJECT

public:
    BookmarkEditBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~BookmarkEditBase();

    QLineEdit* location;
    QLabel* TextLabel1;
    QLineEdit* title;
    QLabel* TextLabel2;
    QPushButton* okButton;
    QPushButton* cancelButton;

protected:
    QGridLayout* BookmarkEditBaseLayout;
};

#endif // BOOKMARKEDITBASE_H
#endif
