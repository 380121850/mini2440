#ifndef KIO_FTP_H
#define KIO_FTP_H

#include <ftp.h>

namespace KIO
{
    class FtpSlave : public ::Ftp
    {
    public:
        FtpSlave();

        virtual void get( const KURL &url );

        virtual void error( int errid, const QString &text );

        virtual void listEntry( const UDSEntry &entry, bool ready );

        virtual void finished();

    private:
        void getDirectory( const KURL &url );

        void sendDirectoryListingHeader();
        void sendDirectoryListingFooter();

        void sendString( const QString &text );

        KURL m_currentURL;
        bool m_gettingURL : 1;
        bool m_blockFinished : 1;
    };
};

#endif // KIO_FTP_H
/* vim: et sw=4 ts=4
 */
