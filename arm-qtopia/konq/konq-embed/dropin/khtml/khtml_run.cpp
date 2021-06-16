/* This file is part of the KDE project
 *
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
 *                     2000 Simon Hausmann <hausmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include "khtml_run.h"
#include "khtml_part.h"
#include <kio/job.h>
#include <kdebug.h>

KHTMLRun::KHTMLRun( KHTMLPart *part, khtml::ChildFrame *child, const KURL &url, 
                    const KParts::URLArgs &args, bool hideErrorDialog )
: KRun( url, 0, false, false /* No GUI */ ), m_part( part ),
  m_args( args ), m_child( child ), m_hideErrorDialog( hideErrorDialog )
{
}

void KHTMLRun::foundMimeType( const QString &_type )
{
    QString mimeType = _type; // this ref comes from the job, we lose it when using KIO again
    if ( !m_part->processObjectRequest( m_child, m_strURL, mimeType ) )
    {
        kdDebug() << "KHTMLRun::foundMimeType " << _type << " couldn't open" << endl;
        KRun::foundMimeType( mimeType );
        return;
    }
    m_bFinished = true;
    m_timer.start( 0, true );
}

void KHTMLRun::slotJobResult( KIO::Job *job )
{
    if ( job->error() && m_hideErrorDialog )
    {
        m_part->processObjectRequest( m_child, KURL(), QString::null );
        delete this; // this is extremely evil and relies on only one slot
                     // connected to the signal. uhoh :)
    }
    else
        KRun::slotJobResult( job ); 
}

#include "khtml_run.moc"
