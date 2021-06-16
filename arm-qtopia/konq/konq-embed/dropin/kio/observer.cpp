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

#include "observer.h"
#include "slavebase.h"
#include "passdlg.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <assert.h>

using namespace KIO;

Observer *Observer::s_self = 0;

Observer *Observer::self()
{
    if ( !s_self )
        s_self = new Observer;

    return s_self;
}

bool Observer::openPassDlg( AuthInfo &nfo )
{

    PassDlg dlg( 0, 0, true, 0, i18n( "Please enter username/password" ) + "<br>" + nfo.commentLabel + "<br>" + nfo.comment , nfo.username, QString::null );
    dlg.setCaption( nfo.caption );

    if ( nfo.readOnly )
        dlg.setEnableUserField( false );

    if ( dlg.exec() )
    {
        nfo.username = dlg.user();
        nfo.password = dlg.password();
        nfo.keepPassword = true;
        return true;
    }

    nfo.keepPassword = false;

    return false;
}

int Observer::messageBox( int /*progressId*/, int type, const QString &_text, const QString &caption,
                          const QString &buttonYes, const QString &buttonNo )
{
    // a next hack to enforce QMessageBox using QRichText for the text. This
    // enables a proper linebreaking of the messages.
    QString text =  "<html>" + _text + "</html>";
    switch ( type )
    {
        case KIO::SlaveBase::QuestionYesNo:
            return KMessageBox::questionYesNo( 0, text, caption, buttonYes, buttonNo );
        case KIO::SlaveBase::WarningYesNoCancel:
            return KMessageBox::warningYesNoCancel( 0, text, caption, buttonYes, buttonNo );
        case KIO::SlaveBase::WarningYesNo:
            return KMessageBox::warningYesNo( 0, text, caption, buttonYes, buttonNo );
        case KIO::SlaveBase::WarningContinueCancel:
            return KMessageBox::warningContinueCancel( 0, text, caption,
                                              !buttonYes.isEmpty() ? buttonYes : i18n( "Continue" ) );
        case KIO::SlaveBase::Information:
            KMessageBox::information( 0, text, caption );
            return 1;
        case KIO::SlaveBase::SSLMessageBox:
            // ### FIXME
            return 1;
        default:
            assert( 0 ); // ### FIXME
    }
}

