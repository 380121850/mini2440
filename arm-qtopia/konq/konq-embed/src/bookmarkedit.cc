#include <config.h>
#if defined(ENABLE_BOOKMARKS)
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../../../src/kdenox/konq-embed/src/bookmarkedit.ui'
**
** Created: Mon Mar 11 10:36:22 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "bookmarkedit.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a BookmarkEditBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
BookmarkEditBase::BookmarkEditBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "BookmarkEditBase" );
    resize( 289, 232 ); 
    BookmarkEditBaseLayout = new QGridLayout( this ); 
    BookmarkEditBaseLayout->setSpacing( 6 );
    BookmarkEditBaseLayout->setMargin( 11 );

    location = new QLineEdit( this, "location" );

    BookmarkEditBaseLayout->addMultiCellWidget( location, 3, 3, 0, 3 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( i18n( "Title:" ) );

    BookmarkEditBaseLayout->addMultiCellWidget( TextLabel1, 0, 0, 0, 1 );

    title = new QLineEdit( this, "title" );

    BookmarkEditBaseLayout->addMultiCellWidget( title, 1, 1, 0, 3 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( i18n( "Location:" ) );

    BookmarkEditBaseLayout->addWidget( TextLabel2, 2, 0 );

    okButton = new QPushButton( this, "okButton" );
    okButton->setText( i18n( "&OK" ) );

    BookmarkEditBaseLayout->addMultiCellWidget( okButton, 5, 5, 1, 2 );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setText( i18n( "&Cancel" ) );

    BookmarkEditBaseLayout->addWidget( cancelButton, 5, 3 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    BookmarkEditBaseLayout->addItem( spacer, 5, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    BookmarkEditBaseLayout->addItem( spacer_2, 4, 2 );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
BookmarkEditBase::~BookmarkEditBase()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "bookmarkedit.moc"
#endif
