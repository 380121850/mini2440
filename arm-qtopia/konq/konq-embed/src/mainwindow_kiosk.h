/* This file is part of the KDE project
   $Id: mainwindow_kiosk.h,v 1.2 2001/07/24 16:46:41 hausmann Exp $
*/

#ifndef __mainwindow_kiosk_h__
#define __mainwindow_kiosk_h__

#include <config.h>

#if defined(KONQ_GUI_KIOSK)

#include "mainwindowbase.h"

class MainWindowKiosk : public MainWindowBase
{
    Q_OBJECT
public:
    MainWindowKiosk();

protected slots:
    virtual void popupMenu( const QString &url, const QPoint & );

protected:
    virtual View *createNewViewInternal( const KURL &url, const KParts::URLArgs &args );

    virtual void initGUI();
};

#endif

#endif // __mainwindow_kiosk_h__
