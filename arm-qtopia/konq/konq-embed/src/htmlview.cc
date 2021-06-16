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


#include "htmlview.h"

#include <qtimer.h>

#include <kglobal.h>
#include <kio/jobclasses.h>
#include <kdatastream.h>

#include <khtmlview.h>
#include <kstdaction.h>

// ###
#include <xml/dom_docimpl.h>
#include <rendering/render_object.h>
#include <misc/loader.h>
#include <jsextconnect.h>

#include <assert.h>
#include <kdebug.h>

HTMLView::HTMLView( QWidget *parentWidget, const char *widgetName,
                    QObject *parent, const char *name )
    : KHTMLPart( parentWidget, widgetName, parent, name )
{
    m_imageMode = false;
    m_utilityMode = name && ( name[0] == '_' );
    m_serviceType = "";
    connect( this, SIGNAL( popupMenu( const QString &, const QPoint & ) ),
	     this, SLOT( popupMenu( const QString &, const QPoint & ) ) );
    if ( parentPart() )
    {
	// This is a frame - inherit some properties and actions
	setScalingFactor( parentPart()->scalingFactor() );
	if ( parentPart()->inherits( "HTMLView" ) )
	    connect( this, SIGNAL( popupMenu( KHTMLPart *, const QString & ) ),
		     parentPart(), SIGNAL( popupMenu( KHTMLPart *, const QString & ) ) );
    }
}

bool HTMLView::openURL( const KURL &url )
{
    static const QString &html = KGlobal::staticQString( "<html><body style=\"margin: 0px\"><img src=\"%1\"></body></html>" );
    static const QString &embed = KGlobal::staticQString( "<html><body style=\"margin: 0px\"><embed src=\"%1\" type=\"%2\" width=\"100%%\" height=\"100%%\"></body></html>" );
    
    QString serviceType = browserExtension()->urlArgs().serviceType;
    if ( !serviceType.isEmpty() )
	m_serviceType = serviceType;
    kdDebug() << "HTMLView::openURL " << url.url() << " type " << m_serviceType << endl;
    const KIO::MimeHandler* mime = KIO::MimeHandler::Find( m_serviceType );
    if ( !m_serviceType.isEmpty() )
        m_imageMode = m_serviceType.startsWith("image/");

    if ( !(m_imageMode || ( mime && mime->isPart() ) ) )
	return KHTMLPart::openURL( url );

    m_url = url;

    KParts::URLArgs args = browserExtension()->urlArgs();

    begin( m_url, args.xOffset, args.yOffset );

    DOM::DocumentImpl *impl = static_cast<DOM::DocumentImpl *>( document().handle() ); // ### hack ;-)
    if ( impl && browserExtension()->urlArgs().reload )
        impl->docLoader()->setReloading( true );

    if ( m_imageMode )
	write( html.arg( m_url.url() ) );
    else
	write( embed.arg( m_url.url() ).arg( m_serviceType ) );
    end();

    KIO::Job *job = khtml::Cache::loader()->jobForRequest( m_url.url() );

    if ( job )
    {
        emit started( job );

        connect( job, SIGNAL( result( KIO::Job * ) ),
                 this, SLOT( slotImageJobFinished( KIO::Job * ) ) );
    }
    else
    {
        emit started( 0 );
        emit completed();
    }

    return true;
}

void HTMLView::saveState( QDataStream &stream )
{
    stream << m_imageMode << m_serviceType;
    KHTMLPart::saveState( stream );
}

void HTMLView::restoreState( QDataStream &stream )
{
    stream >> m_imageMode >> m_serviceType;
    KHTMLPart::restoreState( stream );
}

void HTMLView::slotImageJobFinished( KIO::Job *job )
{
    assert( m_imageMode );

    if ( job->error() )
    {
        job->showErrorDialog();
        emit canceled( job->errorString() );
    }
    else
    {
        if ( view()->contentsY() == 0 )
        {
            KParts::URLArgs args = browserExtension()->urlArgs();
            view()->setContentsPos( args.xOffset, args.yOffset );
        }

        emit completed();
    }
}

void HTMLView::popupMenu( const QString &_url, const QPoint & )
{
    const QObject *from = sender();
    if ( from && from->inherits( "KHTMLPart" ) )
	emit popupMenu( (KHTMLPart*) from, _url );
}

KParts::ReadOnlyPart *HTMLView::createPart( QWidget *parentWidget, const char *widgetName,
                                            QObject *parent, const char *name,
                                            const QString &mimetype, QString &serviceName,
                                            QStringList &serviceTypes, const QStringList &params )
{
    assert( !m_imageMode );

    if ( mimetype.isEmpty() )
        return 0;

    kdDebug() << "HTMLView::createPart service=" << serviceName << " type=" << mimetype
	      << " widgetName=" << widgetName << " name=" << name << endl;

    // Take filters and aliases into account
    const KIO::MimeHandler* mime = KIO::MimeHandler::Find( mimetype );
    QString mtype = mimetype;
    if ( mime ) {
	if ( mime->isAlias() )
	    mtype = mime->isAlias()->outputType();
	else if ( mime->isFilter() )
	    mtype = mime->isFilter()->outputType();
    }
    if ( mtype != mimetype )
	mime = KIO::MimeHandler::Find( mtype );

    if ( !mime )
	return 0;

    KParts::ReadOnlyPart *res;
    if ( mime->isPart() )
    {
	KParts::Factory *factory = ((KIO::MimePart*)mime)->getFactory();
	if ( !factory )
	    return 0;

	if ( serviceName.isEmpty() )
	    serviceName = QString::fromLatin1( "KParts::ReadOnlyPart" );

        res = static_cast<KParts::ReadOnlyPart *>(
	    factory->createPart( parentWidget, widgetName, parent, name,
				 serviceName.latin1(), params ) );
    }
    else if ( mime->isInternal() )
    {
        res = new HTMLView( parentWidget, widgetName, parent, name );
        serviceName = QString::fromLatin1( "HTMLView" );
    }
    else return 0;

    serviceTypes = QStringList() << mtype;

    return res;
}

void HTMLView::begin( const KURL &url, int xOffset, int yOffset )
{
    KHTMLPart::begin( url, xOffset, yOffset );
    if ( m_utilityMode || m_imageMode )
	setAutoloadImages( true );
    if ( m_utilityMode )
    {
	setJScriptEnabled( true );
	setPluginsEnabled( true );
    }
#if defined(ENABLE_JS_HOSTEXTEND)
    if ( jScriptEnabled() )
    {
	// Credits for this great idea go to Jaksa Vuckovic <jaksa@libero.it>
	executeScript( "" ); // this instantiates a new KJSProxy
	KJS::probeJsExtension( this, url, m_serviceType );
    }
#endif
}
			    
#include "htmlview.moc"
