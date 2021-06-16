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

#include "kaction.h"
#include <kdebug.h>

#include <qtimer.h>
#include <qaction.h>

KAction *KActionCollection::action( const char *name )
{
    return ( name && *name ) ? (KAction*)child( name, "KAction" ) : 0;
}

void KActionCollection::qDisconnect()
{
    QObjectList *list = queryList( "KAction", 0, false );
    QObjectListIt it( *list );
    KAction *ka;
    for ( ; ( ka = (KAction*) it.current() ); ++it )
	ka->qDisconnect();
    delete list;
}

void KActionCollection::insertChild( QObject *obj )
{
    QObject::insertChild( obj );
    // We delay notification because usually the metaobject
    //  is not yet valid at this point
    m_newlyAdded.append( obj );
    if ( m_timer )
    {
	m_timer = false;
	QTimer::singleShot(0, this, SLOT(insertNotify()) );
    }
}

void KActionCollection::removeChild( QObject *obj )
{
    bool notify = !m_newlyAdded.removeRef( obj );
    QObject::removeChild( obj );
    if ( notify )
    {
	if ( obj->inherits( "KAction" ) )
	{
	    ((KAction*)obj)->qDisconnect();
	    emit removed( (KAction*)obj );
	}
	if ( obj->name( 0 ) )
	    emit removed( obj->name( 0 ) );
    }
}

void KActionCollection::insertNotify()
{
    m_timer = true;
    while ( !m_newlyAdded.isEmpty() )
    {
	QObject *obj = m_newlyAdded.take( 0 );
	if ( obj )
	{
	    if ( obj->name( 0 ) )
		emit inserted( obj->name( 0 ) );
	    if ( obj->inherits( "KAction" ) )
		emit inserted( (KAction*)obj );
	}
    }
}

KAction::KAction( const QString &, int, const QObject *receiver, const char *slot,
                  QObject *parent, const char *name, bool connectIt )
    : QObject( parent, name ), m_enabled( true )
{
    if ( connectIt )
	connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::KAction( const QString &, const QString &, int, const QObject *receiver, const char *slot,
                  QObject *parent, const char *name )
    : QObject( parent, name ), m_enabled( true )
{
    connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::KAction( const QString&, const QIconSet&, int, const QObject *receiver, const char *slot,
                  QObject *parent, const char *name )
    : QObject( parent, name ), m_enabled( true )
{
    connect( this, SIGNAL( activated() ), receiver, slot );
}

KAction::~KAction()
{
    // Remove from the collection before the metaobject gets destroyed
    if ( parent() && parent()->inherits( "KActionCollection" ) )
	parent()->removeChild( this );
}

void KAction::setEnabled( bool enable )
{
    m_enabled = enable;
    emit enabled( enable );
}

void KAction::qConnect( QAction *action )
{
    qDisconnect();
    m_qConnected = action;
    action->setEnabled( m_enabled );
    connect( action, SIGNAL( activated() ),
	     this, SLOT( activate() ) );
    connect( this, SIGNAL( enabled(bool) ),
	     action, SLOT( setEnabled(bool) ) );
}

void KAction::qDisconnect()
{
    if ( m_qConnected )
    {
	disconnect( m_qConnected, SIGNAL( activated() ),
		    this, SLOT( activate() ) );
	disconnect( this, SIGNAL( enabled(bool) ),
		    m_qConnected, SLOT( setEnabled(bool) ) );
	m_qConnected = 0;
    }
}

KToggleAction::KToggleAction( const QString &text, int accel,
			      const QObject *receiver, const char *slot,
			      QObject *parent, const char *name, bool connectIt )
    : KAction( text, accel, receiver, slot, parent, name, false ),
      m_checked( false ), m_reenter( false )
{
    if ( connectIt )
	connect( this, SIGNAL( toggled(bool) ), receiver, slot );
}

KToggleAction::KToggleAction( const QString &text, int accel,
			      QObject *parent, const char *name )
    : KAction( text, accel, 0, 0, parent, name, false ),
      m_checked( false ), m_reenter( false )
{
}

void KToggleAction::setChecked( bool checked )
{
    m_checked = checked;
    if ( m_qConnected && m_qConnected->isToggleAction() )
    {
	m_reenter = true;
	m_qConnected->setOn( m_checked );
	m_reenter = false;
    }
}

void KToggleAction::slotChecked( bool checked )
{
    if ( m_checked != checked && !m_reenter )
    {
	m_checked = checked;
	emit toggled( m_checked );
    }
}

void KToggleAction::qConnect( QAction *action )
{
    KAction::qConnect( action );
    if ( action->isToggleAction() )
    {
	action->setOn( m_checked );
	connect( action, SIGNAL( toggled(bool) ),
		 this, SLOT( slotChecked(bool) ) );
    }
    else
	kdWarning() << "Non-toggle QAction \"" << action->name()
		    << "\" mismatches KToggleAction \"" << name() << "\"" << endl;
}

void KToggleAction::qDisconnect()
{
    if ( m_qConnected )
    {
	disconnect( m_qConnected, SIGNAL( toggled(bool) ),
		    this, SLOT( slotChecked(bool) ) );
	KAction::qDisconnect();
    }
}

#include "kaction.moc"
