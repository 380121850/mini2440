#include "kflashplay.h"
#include <kapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description =
    I18N_NOOP("A small Macromedia Flash player");

static const char *version = "v0.1";

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    { 0, 0, 0 }
};

int main(int argc, char **argv)
{
    KAboutData about("kflashplay", I18N_NOOP("KFlashPlay"), version, description, KAboutData::License_GPL, "(C) 2002 Paul Chitescu", 0, 0, "paulc-devel@null.ro");
    about.addAuthor( "Paul Chitescu", 0, "paulc-devel@null.ro" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;

    // see if we are starting with session management
    if (app.isRestored())
        RESTORE(KFlashPlay)
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if ( args->count() == 0 )
        {
	    KFlashPlay *widget = new KFlashPlay;
	    widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++ )
            {
                KFlashPlay *widget = new KFlashPlay;
                widget->show();
		widget->load( args->url( i ) );
            }
        }
        args->clear();
    }

    return app.exec();
}
