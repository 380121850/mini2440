#include <config.h>

#define REFRESH_INTERVAL_MIN 500
#define REFRESH_INTERVAL_DEF 2000
#define REFRESH_INTERVAL_MAX 5000

#define SECTION_NAME "PPP Dialer"

#include "pppdialer.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <mainwindowbase.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qwidgetstack.h>
#include <qaction.h>
#include <qregexp.h>
#include <qtimer.h>
#include <kconfig.h>
#include <klocale.h>
#include <ksock.h>
#include <kapp.h>

#if defined(ENABLE_XMLUI)
#include <kstddirs.h>

#if defined(USE_LARGE_ICONS)
#define ACTION_ICON_PATH "konq-embed/actions/big/"
#else
#define ACTION_ICON_PATH "konq-embed/actions/"
#endif
#else

#include <connect_no.xpm>
#include <connect_creating.xpm>
#include <connect_established.xpm>

#endif // ENABLE_XMLUI

#include <kdebug.h>

DialerDlg::DialerDlg( KPPPAction *action, QWidget* parent )
    : DialerBase( parent, "pppdialer", true, WDestructiveClose )
{
    m_kaction = action;
    m_text = new QLabel( m_switcher );
    m_text->setAlignment( AlignLeft | AlignTop | ExpandTabs | WordBreak );
    m_text->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    m_text->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    m_switcher->addWidget( m_infos, 0 );
    m_switcher->addWidget( m_text, 1 );
    setData( action->getCurrentText(), action->getStatusText() );
    connect( m_kaction, SIGNAL( accountsAvailable() ),
	     this, SLOT( insertAccounts() ) );
    m_kaction->listAccounts();
}

void DialerDlg::setMode( bool connected )
{
    if ( connected )
    {
	m_switcher->raiseWidget( 1 );
	m_action->setText( i18n( "Hangup" ) );
	m_action->setEnabled( true );
    }
    else
    {
	m_switcher->raiseWidget( 0 );
	m_action->setText( i18n( "Dial" ) );
	m_action->setEnabled( m_which->count() != 0 );
    }
}

void DialerDlg::setData( const QString &text, const QString &caption )
{
    if ( text.isEmpty() )
	setMode( false );
    else
    {
	setMode( true );
	m_text->setText( text );
    }
    if ( !caption.isEmpty() )
	setCaption( caption );
}

void DialerDlg::accept()
{
    if ( m_kaction->getCurrentStatus() == KPPPAction::NoConnection )
    {
	QString account = m_which->currentText();
	if ( !account.isEmpty() )
	{
	    KConfig *config = KGlobal::config();
	    KConfigGroupSaver saver( config, QString::fromLatin1( SECTION_NAME ) );
	    config->writeEntry( "LastDialed", account );
	    m_kaction->attemptConnect( account );
	}
    }
    else
	m_kaction->attemptDisconnect();
}

void DialerDlg::insertAccounts()
{
    m_which->clear();
    m_which->insertStringList( m_kaction->getAccounts() );
    {
	KConfig *config = KGlobal::config();
	KConfigGroupSaver saver( config, QString::fromLatin1( SECTION_NAME ) );
	QString last = config->readEntry( "LastDialed" );
	if ( !last.isEmpty() )
	{
	    QListBoxItem *find = m_which->listBox()->findItem( last );
	    if ( find )
		m_which->setCurrentItem( m_which->listBox()->index( find ) );
	}
    }
    if ( m_switcher->visibleWidget() == m_infos )
	m_action->setEnabled( m_which->count() != 0 );
}

static DialerDlg *s_dialer = 0;
static MainWindowBase *s_main = 0;

KPPPAction *KPPPAction::s_action = 0;
QString KPPPAction::s_script = QString::null;
QPixmap *KPPPAction::s_images[3] = { 0, 0, 0 };
int KPPPAction::s_timeout = REFRESH_INTERVAL_DEF;

KPPPAction::KPPPAction()
    : KAction( QString::null, 0, 0, 0, s_main->actionCollection(), "ppp_dial", false ),
      m_lastStat( NoConnection ), m_lastText( QString::null )
{
    m_auto = false;
    m_connect = false;
    m_refresh = new QTimer( this );
    connect( this, SIGNAL( activated() ),
	     this, SLOT( showDialer() ) );
    connect( m_refresh, SIGNAL( timeout() ),
	     this, SLOT( statusRefresh() ) );
}

KPPPAction::~KPPPAction()
{
    delete m_refresh;
}

void KPPPAction::signalHandler( int sigtype )
{
    if ( s_action && NOTIFY_SIGNAL == sigtype )
	QTimer::singleShot( 0, s_action, SLOT( signalEvent() ) );
}

bool KPPPAction::createInstance()
{
    if ( s_action )
	return true;
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "PPP Dialer" ) );
    s_script = config->readEntry( "ScriptFile" );
    if ( !s_script.isEmpty() )
    {
	QWidget *top = KApplication::kApplication()->mainWidget();
	if ( top && top->inherits( "MainWindowBase" ) )
	{
	    s_main = static_cast<MainWindowBase*>( top );
	    s_timeout = config->readNumEntry( "StatusRate", REFRESH_INTERVAL_DEF );
	    if ( s_timeout < REFRESH_INTERVAL_MIN )
		s_timeout = REFRESH_INTERVAL_MIN;
	    if ( s_timeout > REFRESH_INTERVAL_MAX )
		s_timeout = REFRESH_INTERVAL_MAX;
#if defined(ENABLE_XMLUI)
	    static const char * const default_names[3] =
		{ "connect_no", "connect_creating", "connect_established" };
	    for ( int i = _first; i <= _last; i++ )
	    {
		QString icon = config->readEntry( default_names[i],
		    QString::fromLatin1( default_names[i] ).append( ".png" ) );
		if ( !( icon.isEmpty() || icon.startsWith( "/" ) ) )
		    icon = locate( "data", ACTION_ICON_PATH + icon );
		s_images[i] = new QPixmap( icon );
	    }
#else
	    s_images[NoConnection] = new QPixmap( connect_no_xpm );
	    s_images[Connecting]   = new QPixmap( connect_creating_xpm );
	    s_images[Established]  = new QPixmap( connect_established_xpm );
#endif
	    s_action = new KPPPAction;
	    ::signal( NOTIFY_SIGNAL, signalHandler );
	    return true;
	}
	else
	    kdWarning()<< "Dialer found no main window!" <<endl;
    }
    return false;
}

void KPPPAction::fixConnectStatus()
{
    switch ( m_lastStat )
    {
	case NoConnection:
	    m_connect = false;
	    break;
	case Established:
	    m_connect = true;
	    break;
    }
}

void KPPPAction::setCurrentStatus( int status )
{
    if ( _first <= status && status <= _last && status != m_lastStat )
    {
	m_lastStat = status;
	fixConnectStatus();
	if ( m_qConnected && s_images[status] )
	    m_qConnected->setIconSet( QIconSet( *s_images[status] ) );
	if ( s_dialer && Established == m_lastStat )
	    m_refresh->start( s_timeout );
	else
	    m_refresh->stop();
    }
}

const QString KPPPAction::getStatusText() const
{
    switch ( m_lastStat )
    {
	case NoConnection:
	    return i18n( "Disconnected" );
	case Connecting:
	    return i18n( "Connecting" );
	case Established:
	    return i18n( "Established" );
	default:
	    return i18n( "Internal Error" );
    }
}

void KPPPAction::interpretText( const QString &text )
{
    if ( text.isEmpty() )
	return;

    unsigned int skip = 0;
    if ( text.at( 1 ).latin1() == ' ' )
    {
	skip = 2;
	switch ( text.at( 0 ).latin1() )
	{
	    case 'L':
		if ( text.length() <= skip )
		    emit accountsAvailable();
		else
		    m_accounts << text.mid( skip );
		return;
	    case 'N':
		setCurrentStatus( NoConnection );
		m_lastText = QString::null;
		if ( s_dialer )
		    s_dialer->setData( QString::null, getStatusText() );
		return;
	    case 'C':
	    case 'H':
		setCurrentStatus( Connecting );
		break;
	    case 'E':
		setCurrentStatus( Established );
		break;
	    default:
		skip = 0;
	}
    }
    m_lastText = text.mid( skip );
    m_lastText.replace( QRegExp( "\a" ), "\n" );
    if ( s_dialer )
    {
	s_dialer->setData( m_lastText, getStatusText() );
	if ( m_auto && Established == m_lastStat )
	    QTimer::singleShot( 500, s_dialer, SLOT( autoClose() ) );
    }
}

void KPPPAction::readEvent( KSocket *sock )
{
    // TODO: implement it properly; now it's horrible
    char buf[1024];
    int r = ::read( sock->socket(), buf, sizeof( buf ) - 1 );
    if ( r > 0 )
    {
	buf[r] = '\0';
	for ( char *pc = buf; pc && *pc; )
	{
	    char *cr = strchr( pc, '\r' );
	    char *lf = strchr( pc, '\n' );
	    if ( cr )
	    {
		if ( lf + 1 == cr )
		    *cr = 0;
		else if ( !lf || cr < lf )
		    lf = cr;
	    }
	    if ( lf )
		*lf = 0;
	    interpretText( QString::fromLatin1( pc ) );
	    pc = lf ? lf + 1 : 0;
	}
    }
}

void KPPPAction::closeEvent( KSocket *sock )
{
    delete sock;
}

void KPPPAction::signalEvent()
{
    statusRefresh();
    if ( !s_dialer && !m_connect )
    {
	m_auto = true;
	showDialer();
    }
}

bool KPPPAction::scriptExecute( const char *command, const char *arg )
{
    int spair[2];
    if ( 0 == ::socketpair( AF_UNIX, SOCK_STREAM, PF_UNIX, spair ) )
    {
	int i;
	switch( ::fork() )
	{
	    case -1:
		// error
		return false;
	    case 0:
		// child
		::dup2( spair[0], 1 );
		for ( i=getdtablesize(); i >= 3 ; i-- )
		    ::close( i );
		::execl( s_script.latin1(), s_script.latin1(),
			 command, arg, 0 );
		::exit( errno );
	    default:
		// parent
		::close( spair[0] );
		::fcntl( spair[1], F_SETFL, O_NONBLOCK );
		{
		    KSocket *sock = new KSocket( spair[1] );
		    connect( sock, SIGNAL( readEvent( KSocket * ) ),
			     this, SLOT( readEvent( KSocket * ) ) );
		    connect( sock, SIGNAL( closeEvent( KSocket * ) ),
			     this, SLOT( closeEvent( KSocket * ) ) );
		    sock->enableRead( true );
		}
		return true;
	}
    }
    return false;
}

void KPPPAction::attemptConnect( const QString &account )
{
    fixConnectStatus();
    if ( !m_connect )
	m_connect = scriptExecute( "connect", account.latin1() );
}

void KPPPAction::attemptDisconnect()
{
    m_auto = false;
    fixConnectStatus();
    if ( m_connect )
	m_connect = !scriptExecute( "disconnect" );
}

void KPPPAction::listAccounts()
{
    m_accounts.clear();
    scriptExecute( "list" );
}

void KPPPAction::statusRefresh()
{
    scriptExecute( "status" );
}

void KPPPAction::showDialer()
{
    if ( s_dialer )
	return;

    s_dialer = new DialerDlg( this, s_main );
    s_dialer->exec();
    s_dialer = 0;
    m_auto = false;
}

PPPInitializer::PPPInitializer()
{
    QTimer::singleShot( 0, this, SLOT( runInit() ) );
}

void PPPInitializer::runInit()
{
    KPPPAction::createInstance();
    delete this;
}

extern "C" void konqe_register_libpppdialer()
{
    // postpone initialization until main window is created
    new PPPInitializer;
}

#include "pppdialer.moc"
