#ifndef __kfiledialog_h__
#define __kfiledialog_h__

#include <config.h>

#include <qfiledialog.h>
#if defined(ENABLE_READWRITE)
#include <kurl.h>
#endif

class KFileDialog
{
public:
    static QString getOpenFileName( const QString &dir = QString::null,
                                    const QString &filter = QString::null,
                                    QWidget *parent = 0, const QString &caption = QString::null );
#if defined(ENABLE_READWRITE)
    static QString getSaveFileName( const QString &dir = QString::null, 
                                    const QString &filter = QString::null,
                                    QWidget *parent = 0, const QString &caption = QString::null );
    static KURL getSaveURL( const QString &dir= QString::null,
                            const QString &filter= QString::null,
                            QWidget *parent= 0, const QString &caption= QString::null )
	{ return getSaveFileName( dir, filter, parent, caption ); }
#endif
};

#if defined(_QT_QPE_) || defined(Q_WS_WIN32)

#if defined(_QT_QPE_)
#include <qpe/fileselector.h>
#else
struct DocLnk
{
    QString file() const { return QString::null; }
};
#endif

class QPEFileSelectorWrapper : public QDialog
{
    Q_OBJECT
public:
    QPEFileSelectorWrapper( QWidget *parent, const char *name = 0 );
    virtual ~QPEFileSelectorWrapper();

    QString selectedPath() const { return m_path; }

private slots:
    void fileSelected( const DocLnk &doc );

private:
    QString m_path;
};

#endif

#endif
