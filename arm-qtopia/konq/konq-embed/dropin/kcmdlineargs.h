#ifndef __kcmdlineargs_h__
#define __kcmdlineargs_h__

// really dummy wrapper, to make cookieserver compile ;-)

#include <qstring.h>

struct KCmdLineOptions
{
    const char *name;
    const char *description;
    const char *def;
};

class KCmdLineArgs
{
public:
    KCmdLineArgs() {};
    ~KCmdLineArgs() {};

    static void init( int, char **, const char *, const char *, const char *, bool ) {}

    static void addCmdLineOptions( const KCmdLineOptions * ) {}

    static KCmdLineArgs *parsedArgs()
        {
            if ( !s_self )
                s_self = new KCmdLineArgs;
            return s_self;
        }

    bool isSet( const QString & ) { return false; }

    QString getOption( const QString & ) { return QString::null; }

private:
    static KCmdLineArgs *s_self;
};

#endif
