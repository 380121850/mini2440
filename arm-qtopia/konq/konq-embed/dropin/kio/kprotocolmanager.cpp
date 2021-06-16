/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 1999 Torben Weis <weis@kde.org>

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

#include "kprotocolmanager.h"
#include <kglobal.h>
#include <kconfig.h>
#include <ioslave_defaults.h>
#include <kdeversion.h>

#include <stdlib.h>

// CACHE SETTINGS
#define DEFAULT_MAX_CACHE_SIZE          5120          //  5 MB
#define DEFAULT_MAX_CACHE_AGE           60*60*24*14   // 14 DAYS
#define DEFAULT_EXPIRE_TIME             60*30         // 1/2 hour

// MAXIMUM VALUE ALLOWED WHEN CONFIGURING
// REMOTE AND PROXY SERVERS CONNECTION AND
// RESPONSE TIMEOUTS.
#define MAX_RESPONSE_TIMEOUT            360           //  6 MIN
#define MAX_CONNECT_TIMEOUT             360           //  6 MIN
#define MAX_PROXY_CONNECT_TIMEOUT       120           //  2 MIN

#ifdef _WS_QWS_
#define DEFAULT_USERAGENT_STRING \
   QString::fromLatin1("Mozilla/5.0 (compatible; Konqueror/%1; QtEmbedded/%2x%3)") \
   .arg( KDE_VERSION_STRING ) \
   .arg( qApp->desktop()->width() ) \
   .arg( qApp->desktop()->height() )
#else
#define DEFAULT_USERAGENT_STRING \
   QString::fromLatin1("Mozilla/5.0 (compatible; Konqueror/%1; X11)").arg( KDE_VERSION_STRING )
#endif

QString KProtocolManager::userAgentForHost( const QString &/*host*/ )
{
    QString agent = QString::fromLatin1( getenv( "HTTP_USER_AGENT" ) );

    if ( agent.isEmpty() )
        agent = DEFAULT_USERAGENT_STRING;

    return agent;
}

bool KProtocolManager::useProxy()
{
    return !httpProxy().isEmpty();
}

QString KProtocolManager::httpProxy()
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "Network Settings" ) );

    /*
    QString res = QString::fromLatin1( getenv( "HTTP_PROXY" ) );

    if ( res.isEmpty() )
        res = QString::fromLatin1( getenv( "http_proxy" ) );

    return res;
    */
    return config->readEntry( "HTTPProxyServer" );
}

void KProtocolManager::setHTTPProxy( const QString &server )
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "Network Settings" ) );
    config->writeEntry( "HTTPProxyServer", server );
}

QString KProtocolManager::ftpProxy()
{
    // ###
    return QString::null;
}

QString KProtocolManager::noProxyFor()
{
    return QString::fromLatin1( getenv( "NO_PROXY_FOR" ) );
}

QString KProtocolManager::proxyFor( const QString &protocol )
{
    if ( protocol.left( 4 ) == "http" )
        return httpProxy();
    else
        return QString::null;
}

bool KProtocolManager::useCache()
{
    return getenv( "KIO_HTTP_USECACHE" ) != 0;
}

int KProtocolManager::maxCacheAge()
{
    return configValue( "KIO_HTTP_MAXCACHEAGE", DEFAULT_MAX_CACHE_AGE );
}

int KProtocolManager::maxCacheSize()
{
    return configValue( "KIO_HTTP_MAXCACHESIZE", DEFAULT_MAX_CACHE_SIZE );
}

int KProtocolManager::proxyConnectTimeout()
{
    return configValue( "KIO_HTTP_PROXY_CONNECT_TIMEOUT", DEFAULT_PROXY_CONNECT_TIMEOUT );
}

int KProtocolManager::connectTimeout()
{
    return configValue( "KIO_HTTP_CONNECT_TIMEOUT", DEFAULT_CONNECT_TIMEOUT );
}

int KProtocolManager::responseTimeout()
{
    return configValue( "KIO_HTTP_RESPONSE_TIMEOUT", DEFAULT_RESPONSE_TIMEOUT );
}

int KProtocolManager::readTimeout()
{
    return configValue( "KIO_HTTP_READ_TIMEOUT", DEFAULT_READ_TIMEOUT );
}

int KProtocolManager::configValue( const char *envVarName, int defaultVal )
{
    QString valStr = QString::fromLatin1( getenv( envVarName ) );
    bool ok = false;

    int val = valStr.toInt( &ok );

    if ( !valStr.isEmpty() && ok )
        return val;

    return defaultVal;
}

KIO::MetaData KProtocolManager::protocolConfig( const QString &protocol )
{
    KIO::MetaData res;
    KConfig *config = KGlobal::config();

    QString group = protocol.lower() + " Settings";

    KConfigGroupSaver saver( config, group );
   
    res.insert( "UseProxy", proxyFor( protocol ) );                 

    res += config->entryMap( group ); 

    // arghl, temporary hack (need kio::sessiondata, kind of)
    if ( !res.contains( "Cookies" ) )
        res.insert( "Cookies", "true" );

    return res;
/*
    KIO::MetaData res;
    res.insert( "UseProxy", proxyFor( protocol ) );                 

    // ### hack
    if ( protocol.startsWith( "http" ) )
    {
        res.insert( "UseCache", useCache() ? "true" : "false" );
        res.insert( "MaxCacheAge", QString::number( maxCacheAge() ) );
    }

    return res;
*/
}

QString KProtocolManager::externalProtocolHandler( const QString &protocol )
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "External Protocols" ) );

    return config->readEntry( protocol, QString::null );
}

