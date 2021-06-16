#ifndef __kio_observer_h__
#define __kio_observer_h__

#include <qstring.h>

#include <kio/global.h>
#include <kio/authinfo.h>

namespace KIO
{

    class Observer
    {
    public:
        Observer() {}
        ~Observer() {}

        static Observer *self();

        bool openPassDlg( AuthInfo &nfo );

        int messageBox( int progressId, int type, const QString &text, const QString &caption,
                        const QString &buttonYes, const QString &buttonNo );

    private:
        static Observer *s_self;
    };

};

#endif
