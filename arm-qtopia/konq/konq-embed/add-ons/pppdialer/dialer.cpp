#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../../../konq-embed/add-ons/pppdialer/dialer.ui'
**
** Created: Mon Jan 13 20:08:16 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "dialer.h"

#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DialerBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DialerBase::DialerBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "DialerBase" );
    resize( 232, 149 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, sizePolicy().hasHeightForWidth() ) );
    setCaption( i18n( "Dialer" ) );
    setSizeGripEnabled( FALSE );
    DialerBaseLayout = new QVBoxLayout( this ); 
    DialerBaseLayout->setSpacing( 6 );
    DialerBaseLayout->setMargin( 11 );

    m_switcher = new QWidgetStack( this, "m_switcher" );
    m_switcher->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, m_switcher->sizePolicy().hasHeightForWidth() ) );

    m_infos = new QFrame( m_switcher, "m_infos" );
    m_infos->setGeometry( QRect( 11, 11, 195, 64 ) ); 
    m_infos->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, m_infos->sizePolicy().hasHeightForWidth() ) );
    m_infos->setFrameShape( QFrame::NoFrame );
    m_infos->setFrameShadow( QFrame::Raised );
    m_infosLayout = new QVBoxLayout( m_infos ); 
    m_infosLayout->setSpacing( 6 );
    m_infosLayout->setMargin( 0 );

    m_label1 = new QLabel( m_infos, "m_label1" );
    m_label1->setText( i18n( "You are currently not connected" ) );
    m_infosLayout->addWidget( m_label1 );

    m_label2 = new QLabel( m_infos, "m_label2" );
    m_label2->setText( i18n( "Choose account to dial:" ) );
    m_infosLayout->addWidget( m_label2 );

    m_which = new QComboBox( FALSE, m_infos, "m_which" );
    m_infosLayout->addWidget( m_which );
    DialerBaseLayout->addWidget( m_switcher );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    m_action = new QPushButton( this, "m_action" );
    m_action->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, m_action->sizePolicy().hasHeightForWidth() ) );
    m_action->setMinimumSize( QSize( 88, 0 ) );
    m_action->setText( QString::null );
    m_action->setOn( FALSE );
    m_action->setDefault( FALSE );
    m_action->setOn( FALSE );
    m_action->setFlat( FALSE );
    Layout1->addWidget( m_action );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_2 );

    m_close = new QPushButton( this, "m_close" );
    m_close->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, m_close->sizePolicy().hasHeightForWidth() ) );
    m_close->setMinimumSize( QSize( 88, 0 ) );
    m_close->setText( i18n( "Close" ) );
    Layout1->addWidget( m_close );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_3 );
    DialerBaseLayout->addLayout( Layout1 );

    // signals and slots connections
    connect( m_action, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_close, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( m_action, m_close );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DialerBase::~DialerBase()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "dialer.moc"
