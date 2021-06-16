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

#include "kstdaction.h"
#include "kaction.h"
#include "klocale.h"
#include <qnamespace.h>
#include <string.h>

// Please keep arrays multiple of typicall word size (4 bytes). Thanks.
typedef struct {
    const char stdname[6*4];
    const char stdlabel[5*4];
    int stdaccel;
    bool toggle;
} std_template;

static const std_template templates[KStdAction::_std_last - KStdAction::New] = {
    { "file_new", I18N_NOOP("&New"), Qt::CTRL+Qt::Key_N, false },
    { "file_open", I18N_NOOP("&Open..."), Qt::CTRL+Qt::Key_O, false },
    { "file_save", I18N_NOOP("&Save"), Qt::CTRL+Qt::Key_S, false },
    { "file_save_as", I18N_NOOP("Save &As..."), 0, false },
    { "file_close", I18N_NOOP("&Close"), Qt::CTRL+Qt::Key_W, false },
    { "file_print", I18N_NOOP("&Print..."), Qt::CTRL+Qt::Key_P, false },
    { "file_quit", I18N_NOOP("&Quit"), Qt::CTRL+Qt::Key_Q, false },
    { "edit_undo", I18N_NOOP("Und&o"), Qt::CTRL+Qt::Key_Z, false },
    { "edit_redo", I18N_NOOP("Re&do"), Qt::SHIFT+Qt::CTRL+Qt::Key_Z, false },
    { "edit_cut", I18N_NOOP("C&ut"), Qt::CTRL+Qt::Key_X, false },
    { "edit_copy", I18N_NOOP("&Copy"), Qt::CTRL+Qt::Key_C, false },
    { "edit_paste", I18N_NOOP("&Paste"), Qt::CTRL+Qt::Key_V, false },
    { "edit_select_all", I18N_NOOP("Select &All"), Qt::CTRL+Qt::Key_A, false },
    { "edit_find", I18N_NOOP("&Find..."), Qt::CTRL+Qt::Key_F, false },
    { "view_zoom", I18N_NOOP("&Zoom..."), 0, false },
    { "view_redisplay", I18N_NOOP("&Redisplay"), Qt::Key_F5, false },
    { "go_back", I18N_NOOP("&Back"), Qt::ALT+Qt::Key_Left, false },
    { "go_forward", I18N_NOOP("&Forward"), Qt::ALT+Qt::Key_Right, false },
    { "go_home", I18N_NOOP("&Home"), Qt::CTRL+Qt::Key_Home, false },
    { "bookmark_add", I18N_NOOP("&Add Bookmark"), Qt::CTRL+Qt::Key_B, false },
    { "bookmark_edit", I18N_NOOP("&Edit Bookmarks..."), 0, false },
    { "options_show_toolbar", I18N_NOOP("Show &Toolbar"), 0, true },
    { "options_show_statusbar", I18N_NOOP("Show St&atusbar"), 0, true },
    { "options_configure", I18N_NOOP("&Configure..."), 0, false },
    { "help", I18N_NOOP("&Help"), Qt::Key_F1, false },
};

KAction *KStdAction::action( StdAction act_enum, const QObject *recvr,
			     const char *slot, QObject *parent,
			     const char *name )
{
    const std_template *tmpl = templates + (act_enum - New);
    if ( tmpl->toggle )
        return new KToggleAction( QObject::tr( tmpl->stdlabel ), tmpl->stdaccel,
				  recvr, slot, parent, name ? name : tmpl->stdname );
    else
	return new KAction( QObject::tr( tmpl->stdlabel ), tmpl->stdaccel, recvr,
			    slot, parent, name ? name : tmpl->stdname );
}

const char *KStdAction::stdName( StdAction act_enum )
{
    return templates[act_enum - New].stdname;
}

QString KStdAction::stdText( int act_enum )
{
    return QObject::tr( templates[act_enum - New].stdlabel );
}

bool KStdAction::stdToggle( int act_enum )
{
    return templates[act_enum - New].toggle;
}

int KStdAction::stdAccel( int act_enum )
{
    return templates[act_enum - New].stdaccel;
}

int KStdAction::stdAction( const char *name )
{
    for ( int i = New; i < _std_last; i++ )
	if ( strcmp( name, templates[i - New].stdname ) == 0 )
	    return i;

    return 0;
}
