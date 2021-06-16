#ifndef __kimageio_h__
#define __kimageio_h__

#include <qstringlist.h>

class KImageIO
{
public:
    enum Mode { Reading, Writing };

    static QStringList mimeTypes( Mode ) { return QStringList(); }

    static void registerFormats() {}

};

#endif
