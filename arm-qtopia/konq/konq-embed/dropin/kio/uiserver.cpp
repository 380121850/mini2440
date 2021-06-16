/*  This file is part of the KDE project
    Copyright (C) 2002 Paul Chitescu <paulc-devel@null.ro>

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

#include <dcopclient.h>
#include <kssl/ksslinfodlg.h>

#include "uiserver.h"


// DCOP function
void UIServer::showSSLInfoDialog(const QString &url, const KIO::MetaData &meta)
{
    KSSLInfoDlg *kid = new KSSLInfoDlg( meta["ssl_in_use"].upper()=="TRUE", 0L /*parent?*/, 0L, true );
    kid->setup( meta["ssl_peer_cert_subject"],
		meta["ssl_peer_cert_issuer"],
		meta["ssl_peer_ip"],
		url,
		meta["ssl_cipher"],
		meta["ssl_cipher_desc"],
		meta["ssl_cipher_version"],
		meta["ssl_cipher_used_bits"].toInt(),
		meta["ssl_cipher_bits"].toInt(),
		KSSLCertificate::KSSLValidation(meta["ssl_cert_state"].toInt()),
		meta["ssl_good_from"],
		meta["ssl_good_until"] );
    kid->exec(); // will delete itself
}


#include "uiserver.moc"
