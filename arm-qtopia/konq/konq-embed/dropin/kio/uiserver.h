#ifndef __kio_uiserver_h__
#define __kio_uiserver_h__

#include <dcopobject.h>
#include <kio/global.h>

class UIServer : public DCOPObject
{
    Q_OBJECT
    K_DCOP
public:
    UIServer() {}
    virtual ~UIServer() {}

k_dcop:
    void showSSLInfoDialog(const QString &url, const KIO::MetaData &data);
};

#endif // __kio_uiserver_h__
