#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file '../../../../src/kdenox/konq-embed/src/preferences.ui'
**
** Created: Mon Mar 11 10:36:21 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "preferences.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a PreferencesBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PreferencesBase::PreferencesBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PreferencesBase" );
    resize( 298, 308 ); 
    setCaption( i18n( "Preferences" ) );
    PreferencesBaseLayout = new QVBoxLayout( this ); 
    PreferencesBaseLayout->setSpacing( 6 );
    PreferencesBaseLayout->setMargin( 11 );

    tabWidget = new QTabWidget( this, "tabWidget" );

    Widget4 = new QWidget( tabWidget, "Widget4" );
    Widget4Layout = new QGridLayout( Widget4 ); 
    Widget4Layout->setSpacing( 6 );
    Widget4Layout->setMargin( 11 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Widget4Layout->addItem( spacer, 3, 1 );

    TextLabel5 = new QLabel( Widget4, "TextLabel5" );
    TextLabel5->setText( i18n( "Home Page:" ) );

    Widget4Layout->addWidget( TextLabel5, 0, 0 );

    homePage = new QLineEdit( Widget4, "homePage" );

    Widget4Layout->addMultiCellWidget( homePage, 1, 1, 0, 1 );

    currentPage = new QPushButton( Widget4, "currentPage" );
    currentPage->setText( i18n( "Current" ) );

    Widget4Layout->addWidget( currentPage, 1, 2 );

    Line2 = new QFrame( Widget4, "Line2" );
    Line2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Widget4Layout->addMultiCellWidget( Line2, 2, 2, 0, 2 );
    tabWidget->insertTab( Widget4, i18n( "General" ) );

    Widget5 = new QWidget( tabWidget, "Widget5" );
    Widget5Layout = new QGridLayout( Widget5 ); 
    Widget5Layout->setSpacing( 6 );
    Widget5Layout->setMargin( 11 );

    TextLabel3 = new QLabel( Widget5, "TextLabel3" );
    TextLabel3->setText( i18n( "Proxy Server:" ) );

    Widget5Layout->addWidget( TextLabel3, 1, 0 );

    Line4 = new QFrame( Widget5, "Line4" );
    Line4->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Widget5Layout->addMultiCellWidget( Line4, 2, 2, 0, 2 );

    useProxy = new QCheckBox( Widget5, "useProxy" );
    useProxy->setText( i18n( "Use Proxy" ) );

    Widget5Layout->addMultiCellWidget( useProxy, 0, 0, 0, 2 );

    proxyServer = new QLineEdit( Widget5, "proxyServer" );
    proxyServer->setEnabled( FALSE );

    Widget5Layout->addMultiCellWidget( proxyServer, 1, 1, 1, 2 );

    cookiePolicy = new QComboBox( FALSE, Widget5, "cookiePolicy" );

    Widget5Layout->addWidget( cookiePolicy, 3, 2 );

    TextLabel6 = new QLabel( Widget5, "TextLabel6" );
    TextLabel6->setText( i18n( "Cookie Policy:" ) );

    Widget5Layout->addMultiCellWidget( TextLabel6, 3, 3, 0, 1 );

    Line5 = new QFrame( Widget5, "Line5" );
    Line5->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    Widget5Layout->addMultiCellWidget( Line5, 4, 4, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Widget5Layout->addItem( spacer_2, 6, 2 );

    enableJavaScript = new QCheckBox( Widget5, "enableJavaScript" );
    enableJavaScript->setText( i18n( "Enable Javascript" ) );

    Widget5Layout->addMultiCellWidget( enableJavaScript, 5, 5, 0, 2 );
    tabWidget->insertTab( Widget5, i18n( "Network" ) );

    tab = new QWidget( tabWidget, "tab" );
    tabLayout = new QGridLayout( tab ); 
    tabLayout->setSpacing( 6 );
    tabLayout->setMargin( 11 );

    minimumFontSize = new QSpinBox( tab, "minimumFontSize" );

    tabLayout->addWidget( minimumFontSize, 0, 1 );

    TextLabel1 = new QLabel( tab, "TextLabel1" );
    TextLabel1->setText( i18n( "Minimum Font Size:" ) );

    tabLayout->addWidget( TextLabel1, 0, 0 );

    TextLabel2 = new QLabel( tab, "TextLabel2" );
    TextLabel2->setText( i18n( "Default Font:" ) );

    tabLayout->addWidget( TextLabel2, 1, 0 );

    defaultFont = new QComboBox( FALSE, tab, "defaultFont" );

    tabLayout->addWidget( defaultFont, 1, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer_3, 5, 0 );

    TextLabel1_2 = new QLabel( tab, "TextLabel1_2" );
    TextLabel1_2->setText( i18n( "Fixed Font Size:" ) );

    tabLayout->addWidget( TextLabel1_2, 4, 0 );

    fixedFontSize = new QSpinBox( tab, "fixedFontSize" );
    fixedFontSize->setEnabled( FALSE );
    fixedFontSize->setMinValue( 4 );

    tabLayout->addWidget( fixedFontSize, 4, 1 );

    Line4_2 = new QFrame( tab, "Line4_2" );
    Line4_2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    tabLayout->addMultiCellWidget( Line4_2, 2, 2, 0, 1 );

    forceFixedFontSize = new QCheckBox( tab, "forceFixedFontSize" );
    forceFixedFontSize->setText( i18n( "Force Fixed Font Size" ) );

    tabLayout->addMultiCellWidget( forceFixedFontSize, 3, 3, 0, 1 );
    tabWidget->insertTab( tab, i18n( "Appearance" ) );
    PreferencesBaseLayout->addWidget( tabWidget );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    defaultsButton = new QPushButton( this, "defaultsButton" );
    defaultsButton->setText( i18n( "Defaults" ) );
    Layout2->addWidget( defaultsButton );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_4 );

    okButton = new QPushButton( this, "okButton" );
    okButton->setText( i18n( "&OK" ) );
    Layout2->addWidget( okButton );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setText( i18n( "&Cancel" ) );
    Layout2->addWidget( cancelButton );
    PreferencesBaseLayout->addLayout( Layout2 );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( useProxy, SIGNAL( toggled(bool) ), proxyServer, SLOT( setEnabled(bool) ) );
    connect( defaultsButton, SIGNAL( clicked() ), this, SLOT( defaults() ) );
    connect( currentPage, SIGNAL( clicked() ), this, SLOT( makeCurrentSiteHomePage() ) );
    connect( forceFixedFontSize, SIGNAL( toggled(bool) ), fixedFontSize, SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PreferencesBase::~PreferencesBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void PreferencesBase::defaults()
{
    qWarning( "PreferencesBase::defaults(): Not implemented yet!" );
}

void PreferencesBase::makeCurrentSiteHomePage()
{
    qWarning( "PreferencesBase::makeCurrentSiteHomePage(): Not implemented yet!" );
}

#include "preferences.moc"
