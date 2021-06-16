#ifndef __kmimetype_h__
#define __kmimetype_h__

#include <ksharedptr.h>
#include <kurl.h>
#include <qvaluelist.h>
#include <qpixmap.h>

#include <sys/types.h>

class KMimeType : public KShared
{
public:
    KMimeType( const QString &name = QString::null ) : m_name( name ) {}

    typedef KSharedPtr<KMimeType> Ptr;
    typedef QValueList<Ptr> List;

    static Ptr findByURL( const KURL & /* ### */ )
        { return new KMimeType( defaultMimeType() ); }

    QString name() { return m_name; }

    static QString comment( const KURL &, bool ) { return QString::null; }

    static QPixmap pixmapForURL( const KURL &, mode_t, int ) { return QPixmap(); }

    static Ptr mimeType( const char * ) { return new KMimeType; }

    QPixmap pixmap( int ) { return QPixmap(); }

    static const QString &defaultMimeType();

private:
    QString m_name;
};

#endif
