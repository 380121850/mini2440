/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "part.h"

#if defined(ENABLE_READWRITE)

#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

using namespace KParts;


bool ReadWritePart::closeURL()
{
    if ( m_bReadWrite && m_bModified )
    {
	int res = KMessageBox::warningYesNoCancel( widget(),
	    i18n( "The document has been modified.\nDo you want to save it ?" ) );
	switch( res )
	{
	    case KMessageBox::Yes:
		return save() && ReadOnlyPart::closeURL();
	    case KMessageBox::No:
		break;
	    default:
		return false;
	}
    }
    return ReadOnlyPart::closeURL();
}

bool ReadWritePart::saveAs( const KURL & kurl )
{
    if ( kurl.isMalformed() || !kurl.isLocalFile() || !kurl.query().isEmpty() )
    {
	KMessageBox::sorry( widget(), i18n( "Can save only to local files." ) );
	return false;
    }

    if ( QFile::exists( kurl.path() ) &&
	 KMessageBox::warningYesNo( widget(),
	    i18n( "Overwrite existing file ?" ) ) != KMessageBox::Yes )
	return false;

    ReadOnlyPart::closeURL();
    m_url = kurl;
    m_file = m_url.path();
    emit setWindowCaption( m_url.prettyURL() );
    return save();
}

bool ReadWritePart::saveToURL()
{
    if ( m_url.isLocalFile() && m_url.query().isEmpty() )
    {
	setModified( false );
	emit completed();
	return true;
    }
    return false;
}

bool ReadWritePart::save()
{
    if ( m_file.isEmpty() || !m_url.isLocalFile() || !m_url.query().isEmpty() )
    {
	QString fname = m_url.fileName();
	if ( m_url.isLocalFile() && m_url.query().startsWith( "??" ) )
	{
	    fname = m_url.query().mid( 2 );
	    int sl = fname.findRev( '/' );
	    if ( sl >= 0 )
		fname.remove( 0, sl + 1 );
	}
				    
	KURL url = KFileDialog::getSaveURL( QString::fromLatin1( "%1/%2" )
			.arg( QDir::currentDirPath() )
			.arg( fname ) );
	if ( url.isEmpty() )
	    return false;

	return saveAs( url );
    }
    return saveFile() && saveToURL();
}

#include "part_rw.moc"

#endif
