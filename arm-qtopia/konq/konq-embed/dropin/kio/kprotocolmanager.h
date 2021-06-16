#ifndef __kprotocolmanager_h__
#define __kprotocolmanager_h__

#include <qstring.h>

#include <kio/global.h>

class KProtocolManager
{
public:

    static QString userAgentForHost( const QString &host );

    static QString defaultUserAgent( const QString & ) { return userAgentForHost( QString::null ); }

    static bool useProxy();

    static QString httpProxy();

    static QString ftpProxy();

    static QString proxyFor( const QString &protocol );

    static QString noProxyFor();

    static bool useCache();

    static int maxCacheAge();

    static int maxCacheSize();

    static int proxyConnectTimeout();

    static int connectTimeout();

    static int responseTimeout();

    static int readTimeout();

    static bool sendUserAgent() { return true; }

    // ### configurability
    static KIO::CacheControl defaultCacheControl() { return KIO::CC_Verify; }

    // ### support kpac? conditionally?
    static bool hasProxyConfigScript() { return false; }

    static void setHTTPProxy( const QString &server ); // use QString::null to unset

    static KIO::MetaData protocolConfig( const QString &protocol );

    static QString externalProtocolHandler( const QString &protocol );

private:
    static int configValue( const char *envVarName, int defaultVal );
};

#endif
