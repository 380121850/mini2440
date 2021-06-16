/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

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

#include "ksslinfodlg.h"
#include <qmessagebox.h>

#include <ksslx509map.h>
#include <klocale.h>

KSSLInfoDlg::KSSLInfoDlg( bool secureConnection, QWidget *parent, const char *, bool )
{
    m_ssl = secureConnection;
    m_parent = parent;
}

void KSSLInfoDlg::setup( const QString &peername, const QString &/*issuer*/,
			 const QString &ip, const QString &/*url*/,
			 const QString &cipher, const QString &/*cipherdesc*/,
			 const QString &/*sslversion*/,
			 int usedbits, int /*bits*/,
			 KSSLCertificate::KSSLValidation certstate,
			 const QString &/*goodFrom*/, const QString &/*goodUntil*/ )
{
    m_moreInfo = "\n\n";
    m_moreInfo += KSSLCertificate::verifyText( certstate );

    KSSLX509Map cert( peername );
    QString tmp;
    if ( !( tmp = cert.getValue( "O" ) ).isEmpty() )
	m_moreInfo += i18n( "\nOrganization: %1" ).arg( tmp );
    if ( !( tmp = cert.getValue( "L" ) ).isEmpty() )
	m_moreInfo += i18n( "\nLocality: %1" ).arg( tmp );
    if ( !( tmp = cert.getValue( "ST" ) ).isEmpty() )
	m_moreInfo += i18n( "\nState: %1" ).arg( tmp );
    if ( !( tmp = cert.getValue( "C" ) ).isEmpty() )
	m_moreInfo += i18n( "\nCountry: %1" ).arg( tmp );
    if ( !( tmp = cert.getValue( "CN" ) ).isEmpty() )
	m_moreInfo += i18n( "\nCommon Name: %1" ).arg( tmp );

    m_moreInfo += "\n";

    m_moreInfo += i18n( "\nServer IP: %1" ).arg( ip );
    m_moreInfo += i18n( "\nCipher: %1" ).arg( cipher );
    m_moreInfo += i18n( "\nStrength: %1 bits" ).arg( usedbits );
}																							    

void KSSLInfoDlg::exec()
{
    QMessageBox::information( m_parent, i18n( "SSL Information" ),
                              m_ssl ?
                              i18n( "The current connection\n is secured with SSL.%1" ).arg( m_moreInfo )
                              :
                              i18n( "The current connection\n is not secured with SSL." )
        );

    delete this;
}
