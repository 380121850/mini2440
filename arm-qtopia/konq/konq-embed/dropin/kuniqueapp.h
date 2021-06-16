#ifndef __kuniqueapp_h__
#define __kuniqueapp_h__

// hacky-wacky dummy-app object, for cookie-jar server

#include <dcopobject.h>

// for compatibility
#include <kapp.h>

class DCOPClient;

class KUniqueApplication : public DCOPObject
{
    Q_OBJECT
public:
    KUniqueApplication() {}
    ~KUniqueApplication() {}

    void quit() { kapp->quit(); }

    DCOPClient *dcopClient() const { return kapp->dcopClient(); }
};

#endif
