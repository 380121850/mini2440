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

*/


#ifndef __htmlview_h__
#define __htmlview_h__

#include <khtmlview.h>
#include <khtml_part.h>

class HTMLView;

class HTMLView : public KHTMLPart
{
    Q_OBJECT
public:
    HTMLView( QWidget *parentWidget, const char *widgetName,
              QObject *parent, const char *name );

    virtual bool openURL( const KURL &url );

    virtual void saveState( QDataStream &stream );
    virtual void restoreState( QDataStream &stream );

    virtual KParts::ReadOnlyPart *createPart( QWidget *parentWidget, const char *widgetName,
                                              QObject *parent, const char *name,
                                              const QString &mimetype, QString &serviceName,
                                              QStringList &serviceTypes, const QStringList &params );

    bool isUtility() { return m_utilityMode; }

    virtual void begin ( const KURL &url = KURL(), int xOffset = 0, int yOffset = 0 );

private slots:
    void slotImageJobFinished( KIO::Job *job );
    void popupMenu( const QString &_url, const QPoint &pt );

signals:
    void popupMenu( KHTMLPart *part, const QString &_url );

private:
    bool m_imageMode;
    bool m_utilityMode;
    QString m_serviceType;
};

#endif
