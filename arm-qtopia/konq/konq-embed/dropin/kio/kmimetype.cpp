
#include "kmimetype.h"

#include <kglobal.h>

const QString &KMimeType::defaultMimeType()
{
    static const QString &type = KGlobal::staticQString( "application/octet-stream" );
    return type;
}
