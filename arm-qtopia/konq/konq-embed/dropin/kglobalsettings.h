#ifndef __kglobalsettings_h__
#define __kglobalsettings_h__

#include <qfont.h>

#define KDE_DEFAULT_CHANGECURSOR true

class KGlobalSettings
{
public:

    static QFont generalFont();
    static QFont fixedFont();
    static QFont menuFont() { return generalFont(); }
    static QFont windowTitleFont() { return generalFont(); }

    static int dndEventDelay() { return 5; } // ###

    static int contrast() { return 7; } // ###

private:
    static QFont *s_generalFont;
    static QFont *s_fixedFont;
};

#endif
