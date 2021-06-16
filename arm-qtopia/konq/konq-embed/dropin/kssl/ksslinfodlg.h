#ifndef __ksslinfodlg_h__
#define __ksslinfodlg_h__

#include <qstring.h>

#include <ksslcertificate.h>

class QWidget;

class KSSLInfoDlg
{
public:
    KSSLInfoDlg( bool secureConnection, QWidget *parent, const char *, bool );

    void setup( const QString &peername,
                const QString &issuer,
                const QString &ip,
                const QString &url,
                const QString &cipher,
                const QString &cipherdesc,
                const QString &sslversion,
                int usedbits,
                int bits,
                KSSLCertificate::KSSLValidation certstate,
                const QString &goodFrom,
                const QString &goodUntil );

    void exec();

private:
    bool m_ssl;
    QWidget *m_parent;
    QString m_moreInfo;
};

#endif
