#ifndef __kfontdatabase_h__
#define __kfontdatabase_h__

#include <qglobal.h>

#if defined(QT_NO_FONTDATABASE) && defined(_WS_QWS_)

class QFontDatabase
{
public:
    inline bool isSmoothlyScalable( const QString &,
                                    const QString &,
                                    const QString & )
    { return true; }

    inline QValueList<int> smoothSizes( const QString &,
                                        const QString &,
                                        const QString & )
    { return QValueList<int>(); }

    inline QString styleString( const QFont & )
    { return QString::null; }
};

#endif

#endif

