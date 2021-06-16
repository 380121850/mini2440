#ifndef __kglobal_h__
#define __kglobal_h__

#include <qstring.h>

#include <kinstance.h>
#include <kapp.h>

// ### compat
#include <sys/stat.h>

// ### hack
#include <kfontdatabase.h>

class KStringDict;
class KLocale;
class KStandardDirs;
class KCharsets;
class KConfig;

/**
 * A typesafe function to find the minimum of the two arguments.
 */
#define KMIN(a,b)	kMin(a,b)

/**
 * A typesafe function to find the maximum of the two arguments.
 */
#define KMAX(a,b)	kMax(a,b)

template<class T>
inline const T& kMin (const T& a, const T& b) { return a < b ? a : b; }

template<class T>
inline const T& kMax (const T& a, const T& b) { return a > b ? a : b; }

class KGlobal
{
public:

    static const QString &staticQString( const char * );
    static const QString &staticQString( const QString & );

    static KLocale *locale();

    static KStandardDirs *dirs() { return instance()->dirs(); }

    static KConfig *config() { return instance()->config(); }

    static KCharsets *charsets();

    static KInstance *instance() { return kapp; }

    static KInstance *_activeInstance;
    // filled by kapp ctor
    static KInstance *_instance;
private:
    static KStringDict *s_stringDict;
    static KLocale *s_locale;
    static KCharsets *s_charsets;
};

#endif
