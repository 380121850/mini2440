#ifndef __kiconloader_h__
#define __kiconloader_h__

#include <qpixmap.h>
#include <qstring.h>
#include <qmap.h>

#include <kiconeffect.h>

// ###
static inline QPixmap BarIcon( const QString & ) { return QPixmap(); }

class KIconLoader
{
public:
    KIconLoader() {}
    ~KIconLoader() {}

    QPixmap loadIcon( const QString &name, int = 0, int = 0, int = 0 );

    // extension
    void registerIcon( const QString &name, const QPixmap &pix );

    static KIconLoader *self();

private:
    static KIconLoader *s_self;

    QMap<QString,QPixmap> m_pixmaps;
};

inline QPixmap SmallIcon( const char * ) { return QPixmap(); }

#endif
