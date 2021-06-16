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


#include "preferencesimpl.h"
#include "defaults.h"
#include "mainwindowbase.h"
#include "view.h"
#include "htmlview.h"
#include <kio/kprotocolmanager.h>

#include <config.h>

#include <qpushbutton.h>
#include <qfontdatabase.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <khtml_factory.h>
#include <khtml_settings.h>
#include <khtmldefaults.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#if defined(_WS_QWS_)
#include <qfontmanager_qws.h>
#endif

#if defined(_QT_QPE_)
#include <qpe/fontmanager.h>
#endif

namespace
{

int cookieAdviceToIndex( const QString &_advice )
{
    QString advice = _advice.lower();
    if ( advice == "reject" )
        return 1;
    if ( advice == "accept" )
        return 2;
    return 0;
}

QString indexToCookieAdvice( int idx )
{
   if ( idx == 1 )
       return QString::fromLatin1( "Reject" );
   if ( idx == 2 )
       return QString::fromLatin1( "Accept" );
   return QString::fromLatin1( "Ask" );
}

}

Preferences::Preferences( MainWindowBase *parent, const char *name )
	: PreferencesBase( parent, name, true ), m_mainWindow( parent )
{
#if defined(KONQ_GUI_QPE)
    okButton->hide();
    cancelButton->hide();
#endif

    QStringList fontFamilies;
#ifndef QT_NO_FONTDATABASE
    QFontDatabase fontdb;
    fontFamilies = fontdb.families( false );
#else
    if ( !qt_fontmanager )
        QFontManager::initialize();

    QDiskFont *df = qt_fontmanager->diskfonts.first();
    for (; df; df = qt_fontmanager->diskfonts.next() )
        fontFamilies.append( df->name.latin1() );

#endif

#if defined(_QT_QPE_)
    // ### ugly
    if ( fontFamilies.count() == 0 )
    {
        QFont defaultFont;
        fontFamilies.append( defaultFont.family() ); 

        if ( FontManager::hasUnicodeFont() )
        {
            QFont uni1 = FontManager::unicodeFont( FontManager::Proportional );
            QFont uni2 = FontManager::unicodeFont( FontManager::Fixed );
   
            if ( uni1 != defaultFont )
                fontFamilies.append( uni1.family() );
            if ( uni2 != uni1 )
                fontFamilies.append( uni2.family() );
        }
    }
#endif

    KHTMLSettings *settings = KHTMLFactory::defaultHTMLSettings();

    setupFontCombo( defaultFont, fontFamilies, settings->stdFontName() );

    minimumFontSize->setValue( settings->minFontSize() );
    
    forceFixedFontSize->setChecked( settings->fixedFontSize() > 0 );
    fixedFontSize->setValue( settings->fixedFontSize() );

    homePage->setText( BrowserView::homeURL().prettyURL() );

    currentPage->setEnabled( m_mainWindow->currentView() &&
			     m_mainWindow->currentView()->isPlainBrowser() );

    QString httpProxy = KProtocolManager::httpProxy();
    bool proxy = !httpProxy.isEmpty();

    useProxy->setChecked( proxy );
    proxyServer->setText( httpProxy );

    enableJavaScript->setChecked( settings->isJavaScriptEnabled() );

    cookiePolicy->insertItem( i18n( "Ask" ) );
    cookiePolicy->insertItem( i18n( "Reject" ) );
    cookiePolicy->insertItem( i18n( "Accept" ) );

    KConfig *config = KGlobal::config();
    config->setGroup( "Cookie Policy" );
    cookiePolicy->setCurrentItem( cookieAdviceToIndex( config->readEntry( "CookieGlobalAdvice", "Ask" ) ) );    

#if defined(_WS_QWS_)
    if ( MainWindowBase::STYLE_WINDOWED != MainWindowBase::s_winStyle )
	showMaximized();
#endif
}

Preferences::~Preferences()
{
}

void Preferences::save()
{
    KConfig *global = KGlobal::config();

#if 0
    KConfig khtmlrc( "khtmlrc" );
    khtmlrc.setGroup( "HTML Settings" );

    khtmlrc.writeEntry( "StandardFont", defaultFont->currentText() );
    khtmlrc.writeEntry( "FixedFont", defaultFont->currentText() );
    khtmlrc.writeEntry( "SerifFont", defaultFont->currentText() );
    khtmlrc.writeEntry( "SansSerifFont", defaultFont->currentText() );
    khtmlrc.writeEntry( "CursiveFont", defaultFont->currentText() );
    khtmlrc.writeEntry( "FantasyFont", defaultFont->currentText() );

    khtmlrc.writeEntry( "MinimumFontSize", minimumFontSize->value() );
#else
    // for some weird reason khtmlrc is not taking precedence
    // so we modify globals
    global->setGroup( "HTML Settings" );
    global->writeEntry( "StandardFont", defaultFont->currentText() );
/*
    global->writeEntry( "FixedFont", defaultFont->currentText() );
    global->writeEntry( "SerifFont", defaultFont->currentText() );
    global->writeEntry( "SansSerifFont", defaultFont->currentText() );
    global->writeEntry( "CursiveFont", defaultFont->currentText() );
    global->writeEntry( "FantasyFont", defaultFont->currentText() );
*/
    global->writeEntry( "MinimumFontSize", minimumFontSize->value() );
#endif
    global->setGroup( "HTML Settings" );
    // we write this property not into khtmlrc but into konq-embedrc
    // because we have a default in ipkg/konq-embedrc.in
    global->writeEntry( "FixedFontSize", forceFixedFontSize->isChecked() ?
                                         fixedFontSize->value() : 0 );

    global->setGroup( "Java/JavaScript Settings" );
    global->writeEntry( "EnableJavaScript", enableJavaScript->isChecked() );

    QString proxy;
    if ( useProxy->isChecked() )
        proxy = proxyServer->text();
    KProtocolManager::setHTTPProxy( proxy );

    global->setGroup( "General" );
    global->writeEntry( "HomeURL", homePage->text() );
 
    global->setGroup( "Cookie Policy" );
    global->writeEntry( "CookieGlobalAdvice", indexToCookieAdvice( cookiePolicy->currentItem() ) ); 

    global->sync();
}

void Preferences::defaults()
{
    setFontComboEntry( defaultFont, HTML_DEFAULT_VIEW_FONT );

    forceFixedFontSize->setChecked( false );
    fixedFontSize->setValue( fixedFontSize->minValue() );

    minimumFontSize->setValue( HTML_DEFAULT_MIN_FONT_SIZE );

    homePage->setText( DEFAULT_HOMEPAGE );

    useProxy->setChecked( false );

    enableJavaScript->setChecked( false );

    cookiePolicy->setCurrentItem( 0 );
}

void Preferences::makeCurrentSiteHomePage()
{
    View *view = m_mainWindow->currentView();
    if ( !view || !view->part() )
        return;
    homePage->setText( view->part()->url().prettyURL() );
}

void Preferences::setupFontCombo( QComboBox *combo, const QStringList &_items,
                                  const QString &current )
{
    combo->clear();

    QStringList items = _items;
#ifdef _WS_QWS_
    {
        // ### HACK to get rid of the ugly babelfish thingy
        QStringList::Iterator it = items.begin();
        QStringList::Iterator end = items.end();
        for (; it != end; ++it )
            if ( (*it).lower() == QString::fromLatin1( "babelfish" ) )
            {
                items.remove( it );
                break;
            }
    }
#endif
    combo->insertStringList( items );

    setFontComboEntry( combo, current );
}

void Preferences::setFontComboEntry( QComboBox *combo, const QString &item )
{
    for ( int i = 0; i < combo->count(); ++i )
        if ( combo->text( i ).lower() == item.lower() )
        {
            combo->setCurrentItem( i );
            break;
        }
}

#include "preferencesimpl.moc"

/*
 * vim:et
 */

