#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include <qapplication.h>
#include <qtimer.h>

QApplication *app = 0;
pid_t child = -1;
int retcode = 0;

void deadchild( int /*signum*/ )
{
    waitpid( child, &retcode, WNOHANG );
    if ( app )
	QTimer::singleShot( 100, app, SLOT( quit() ) );
}

int main( int argc, char **argv )
{
    if ( argc <= 1 )
    {
	puts("Please give at least one argument.");
	return -1;
    }
    app = new QApplication( argc, argv, QApplication::GuiServer );
    child = vfork();
    switch ( child )
    {
	case 0:
	    usleep( 100000 );
	    execvp( argv[1] , argv+1 );
	    break;
	case -1:
	    return errno;
    }
    signal( SIGCHLD, deadchild );
    app->exec();
    return retcode;
}
