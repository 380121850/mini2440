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

// !! The history code is (c) David Faure <faure@kde.org> !!

#ifndef __view_h__
#define __view_h__

#include <qguardedptr.h>
#include <qobject.h>

#include <kurl.h>
#include <kparts/browserextension.h>
#include <kparts/part.h>
#include <kparts/factory.h>
#include <kaction.h>

class QMainWindow;
class KHTMLPart;
class HTMLView;
class BrowserView;
class Run;

namespace KIO
{
    class Job;
};

class BrowserInterface;

class View : public QObject
{
    Q_OBJECT

public:
    View( QObject *parent, const char *name, KParts::ReadOnlyPart *part = 0 );

    virtual ~View();

    virtual BrowserView* isBrowser() { return 0; }

    virtual void setupActions();

    virtual void openURL( const KURL &url, const QString &mimeType, const KParts::URLArgs &args );

    virtual bool isPlainBrowser() const { return false; }

    bool queryClose() { return m_part->closeURL(); }

    bool showStatus() const { return m_status; }

    QString statusText() const { return m_sbtext; }

    QString caption() const { return m_caption; }

    QString locationBarURL() const { return m_locationBarURL; }

    QString findText() const { return m_findText; }

    KParts::ReadOnlyPart *part() const { return m_part; }
    
    int zoomValue() const { return m_zoom; }

    KActionCollection *actionCollection() { return &m_collection; }

    void setItemVisible( const char *name, bool visible = true );

    bool localVisible( const char *name ) { return m_collection.child( name ); }

    bool itemVisible( const char *name );

public slots:
    virtual void openURL( const KURL &url, const KParts::URLArgs &args = KParts::URLArgs() );

    void setFindText( const QString &text );

    virtual void find() {}

    virtual void findNext() {}

    virtual void zoomTo( int zoomValue );

    virtual void toggleStatusBar( bool visible );

signals:
    void openURLRequest( View *view, const KURL &url, const KParts::URLArgs &args );

    void createPartView( const KURL &url, KParts::Factory *factory, bool rw );

    void captionChanged( const QString &caption );

    void locationBarURLChanged( const QString &url );

    void statusMessage( const QString &text, bool visible );

    void actionChanged( const char * );

protected slots:
    void setLocationBarURL( const QString &url );

    void statusMessage( const QString &text );

    void setCaption( const QString &caption );

private slots:
    void partDestroyed();

protected:
    bool openNewURL( const KURL &url, const KParts::URLArgs &args );

    QString m_locationBarURL;

    KParts::ReadOnlyPart *m_part;

    int m_zoom;

    bool m_status;

private:
    QString m_caption;

    QString m_sbtext;

    KActionCollection m_collection;

    QString m_findText;
};

class BrowserView : public View
{
    Q_OBJECT
public:
    BrowserView( const QString &frameName, QWidget *parentWidget,
	  const char *widgetName, QObject *parent, const char *name, bool status = false );

    virtual ~BrowserView();

    virtual BrowserView* isBrowser() { return this; }

    virtual void setupActions();

    virtual void openURL( const KURL &url, const QString &mimeType, const KParts::URLArgs &args );

    virtual bool isPlainBrowser() const;

    bool canGoBack() { return m_history.at() > 0; }

    bool canGoForward() { return m_history.at() != (int)(m_history.count() - 1); }

    HTMLView *document() const { return m_doc; }

    QString frameName() const { return m_frameName; }

    QStringList frameNames() const;

    bool canHandleFrame( const QString &name, KParts::BrowserHostExtension **hostExtension );

    // same as 'internal' goHistory, but delayed action by a singleshot timer, in order not
    // to fuck up the caller stack if the history change deletes the caller.
    void goHistoryDelayed( int steps );

    uint historyLength() const { return m_history.count(); }

    static QStringList childFrameNames( KParts::ReadOnlyPart *part );

    static KParts::BrowserHostExtension *hostExtension( KParts::ReadOnlyPart *part, const QString &name );

    static KURL homeURL();

    static bool showLocation;

    static bool showStatusBar;

    static int zoomFactor;

public slots:
    virtual void openURL( const KURL &url, const KParts::URLArgs &args = KParts::URLArgs() );

    virtual void zoomTo( int zoomValue );

    virtual void toggleStatusBar( bool visible );

    void toggleLocationBar( bool visible );

    void home();
    void back();
    void forward();
    void reload();
    void stop();

    void find();
    void findNext();
    void findClose();

    void runError();
    void started( KIO::Job *job );
    void completed();

signals:
    void openURLRequest( View *view, const KURL &url, const KParts::URLArgs &args );

    void locationBarURLChanged( const QString &url );

private slots:
    void infoMessage( KIO::Job *, const QString &text );

    void goHistoryFromBuffer();

    void openURLNotify();

protected:
    HTMLView *m_doc;

    KAction *m_stop;
    KAction *m_back;
    KAction *m_forward;

private:
    void setScaling( KHTMLPart *frame );

    void createHistoryEntry();
    void updateHistoryEntry();
    void goHistory( int steps );

    struct HistoryEntry
    {
        KURL m_url;
        QByteArray m_buffer;
    };

    QList<HistoryEntry> m_history;

    QGuardedPtr<Run> m_run;

    QString m_oldLocationBarURL;

    QString m_frameName;

    BrowserInterface *m_browserIface;

    int m_goBuffer;
};

#endif
