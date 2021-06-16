/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

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

   $Id: mainwindow_x11.h,v 1.3.2.2 2002/11/26 11:30:25 pchitescu Exp $
*/

#ifndef __mainwindow_x11_h__
#define __mainwindow_x11_h__

#include <config.h>

#if !defined(KONQ_GUI_KIOSK)

#include "mainwindowbase.h"

class MainWindowX11 : public MainWindowBase
{
    Q_OBJECT
public:
    MainWindowX11();

protected:
    virtual void initGUI();
};

#endif

#endif // __mainwindow_x11_h__
