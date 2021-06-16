#ifndef __KFLASH_H
#define __KFLASH_H

#include <config.h>

#include <kparts/browserextension.h>
#include <kparts/factory.h>
#include <kparts/part.h>
#include <kio/job.h>
#include <klibloader.h>
#include <qwidget.h>
#include <qimage.h>
#include <qtimer.h>
#include <qguardedptr.h>

#include "flashlib/flash.h"

class KAboutData;
class KInstance;
class QLabel;
class KFlashPart;
class KHTMLPart;

class KFlashFactory : public KParts::Factory
{
    Q_OBJECT

public:
    KFlashFactory();
    virtual ~KFlashFactory();

    virtual KParts::Part * createPart( QWidget *parentWidget = 0, const char *widgetName = 0,
				       QObject *parent = 0, const char *name = 0,
				       const char *classname = "KParts::ReadOnlyPart",
				       const QStringList &args = QStringList() );

    static KInstance *instance();

private:
    static KInstance *s_instance;
    static KAboutData* s_about;
};


class KFlashWidget : public QWidget
{
    Q_OBJECT
public:
    KFlashWidget( QWidget *parent=0, const char *name=0 );
    virtual ~KFlashWidget();

    void setPart( KFlashPart *part ) { m_part=part; }
    void setSettings ( int settings ) { m_settings = settings; }
    bool addData( const QByteArray &data, int level = 0 );

    void cbCursorOnOff( int on );

protected:
    virtual bool event( QEvent *ev );
    virtual void resizeEvent( QResizeEvent *ev );
    virtual void paintEvent ( QPaintEvent *ev );

    void execFlash( long flag, FlashEvent *fe = 0 );
    void reCompute();

protected slots:
    void slotTimeout();

signals:
    void resized(int,int);

private:
    void initFlash();

    QByteArray m_data;
    QImage *m_image;
    QTimer m_timer;
    FlashDisplay m_disp;
    KFlashPart *m_part;
    FlashHandle m_handle;
    FlashInfo m_info;
    long m_delay;
    int m_settings;
};


class KFlashPart: public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    KFlashPart( QWidget *parentWidget, const char *widgetName, QObject *parent,
		const char *name, const QStringList &args = QStringList() );
    virtual ~KFlashPart();

    void cbGetUrl( QCString url, QCString target );
    void cbGetSwf( QCString url, int level );

    virtual bool openURL(const KURL &url);
    virtual bool closeURL();
    virtual bool openFile() { return false; };

protected slots:
    void pluginResized(int,int);

private slots:
    void slotData( KIO::Job*, const QByteArray &data );
    void slotFinished( KIO::Job* );

private:
    KIO::TransferJob *m_job;
    KFlashWidget *m_widget;
    KParts::BrowserExtension *m_extension;
    KHTMLPart *m_khtml;
    QString *m_movie;
};

#endif // __KFLASH_H
