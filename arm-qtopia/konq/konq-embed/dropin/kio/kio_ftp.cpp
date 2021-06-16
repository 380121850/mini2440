#include "kio_ftp.h"

#if defined(ENABLE_FTP)

#include <qtextstream.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <kdebug.h>

using namespace KIO;

/*
 * Few of that code is really ftp specific. It could be factored out into a generic wrapper
 * for other ioslaves from KDE, to turn their listDir() into html output.
 */

FtpSlave::FtpSlave()
    : ::Ftp( "", "" ), m_gettingURL( false ), m_blockFinished( false )
{
}

void FtpSlave::get( const KURL &url )
{
    m_gettingURL = true;
    m_currentURL = url;
    ::Ftp::get( url );
    m_gettingURL = false;
    m_currentURL = KURL();
}

void FtpSlave::error( int errid, const QString &text )
{
    if ( !m_gettingURL || errid != ERR_IS_DIRECTORY ) {
        ::Ftp::error( errid, text );
        return;
    }

    getDirectory( m_currentURL );
}

void FtpSlave::listEntry( const UDSEntry &entry, bool ready )
{
    // some parts of the KIO API look stupid...
    if ( ready )
        return;

    bool isDir = false;

    KURL url( m_currentURL );
    QString name;

    for ( UDSEntry::ConstIterator it = entry.begin();
          it != entry.end(); ++it ) {

        switch ( ( *it ).m_uds ) {
            case UDS_FILE_TYPE:
                isDir = S_ISDIR( ( *it ).m_long );
                break;
            case UDS_NAME:
                name = ( *it ).m_str;
                break;
            default: break;
        }
    }

    if ( isDir )
        name += '/';

    url.addPath( name );

    sendString( QString::fromLatin1( "<a href=\"%1\">%2</a>\n" ).
                arg( url.url() ).arg( name ) );
}

void FtpSlave::finished()
{
    if ( m_blockFinished )
        return;

    ::Ftp::finished();
}

void FtpSlave::getDirectory( const KURL &url )
{
    m_blockFinished = true;

    sendDirectoryListingHeader();

    listDir( url );

    sendDirectoryListingFooter();

    m_blockFinished = false;

    finished();
}

void FtpSlave::sendDirectoryListingHeader()
{
    sendString( QString::fromLatin1( "<html><body><pre>" ) );
}

void FtpSlave::sendDirectoryListingFooter()
{
    sendString( QString::fromLatin1( "</pre></body></html>" ) );
}

void FtpSlave::sendString( const QString &text )
{
    QByteArray buffer;
    QTextStream( buffer, IO_WriteOnly ) << text;
    data( buffer );
}

#endif

/* vim: et sw=4 ts=4
 */
