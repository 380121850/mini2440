#ifndef __kmimemagic_h__
#define __kmimemagic_h__

#include <qstring.h>
#include <qcstring.h>

class KMimeMagicResult
{
public:
    QString mimeType() const { return QString::null; }
};

class KMimeMagic
{
public:
    KMimeMagic() {}

    KMimeMagicResult *findBufferFileType( const QByteArray &, const QString & ) { return 0; }

    static KMimeMagic *self();
 
private:
    static KMimeMagic *s_self;
};

#endif

