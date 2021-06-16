#ifndef __kinstance_h__
#define __kinstance_h__

// dummy

class KStandardDirs;
class KConfig;
class KAboutData;

#include <kiconloader.h>

class KInstance
{
    friend class KStandardDirs;
public:
    KInstance( KAboutData *data );
    KInstance( const char *name );
    ~KInstance() {}

    KStandardDirs *dirs() const { return m_dirs; }

    KConfig *config() const;

    QCString instanceName() const { return m_name; }

    KIconLoader *iconLoader() const { return KIconLoader::self(); }

private:
    KStandardDirs *m_dirs;
    mutable KConfig *_config;
    QCString m_name;
};

#endif
