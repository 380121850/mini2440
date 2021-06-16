#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <dcopclient.h>
#include <qlabel.h>
#include <qpainter.h>
#include <string.h>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kconfig.h>

#include "kflashpart.h"
#include "kflashpart.moc"


static bool s_soundEnabled = false;
static bool s_jsEnabled = false;

// Flash library specific callback functions
static void getUrl( char *url, char *target, void *client_data )
{
    if ( client_data )
	((KFlashPart*)client_data)->cbGetUrl( url, target );
}

static void getSwf( char *url, int level, void *client_data )
{
    if ( client_data )
	((KFlashPart*)client_data)->cbGetSwf( url, level );
}

static void cursorOnOff( int on, void *client_data )
{
    if ( client_data )
	((KFlashWidget*)client_data)->cbCursorOnOff( on );
}

class KFlashBrowserExtension : public KParts::BrowserExtension
{
    friend class KFlashPart;
public:
    KFlashBrowserExtension( KParts::ReadOnlyPart *parent,
                          const char *name = 0L )
        : KParts::BrowserExtension( parent, name ) {}
    ~KFlashBrowserExtension() {}
};



KInstance*  KFlashFactory::s_instance = 0L;
KAboutData* KFlashFactory::s_about = 0L;

KFlashFactory::KFlashFactory()
    : KParts::Factory()
{
}

KFlashFactory::~KFlashFactory()
{
   if ( s_instance ) {
       delete s_instance;
       delete s_about;
   }
   s_instance = 0;
}

KParts::Part * KFlashFactory::createPart(QWidget *parentWidget, const char *widgetName,
                                         QObject *parent, const char *name,
                                         const char */*classname*/, const QStringList &args)
{
    kdDebug(1432) << "KFlashFactory::createPart" << endl;
    KParts::Part *obj = new KFlashPart(parentWidget, widgetName, parent, name, args);
    emit objectCreated(obj);
    return obj;
}


KInstance *KFlashFactory::instance()
{
    if ( !s_instance )
    {
	s_about = new KAboutData( "kflashpart", I18N_NOOP("KFlashPart"),
				  PLUGIN_NAME " - " FLASH_VERSION_STRING );
	s_about->addAuthor( "Paul Chitescu", 0, "paulc-devel@null.ro" );
        s_instance = new KInstance( s_about );
    }
    return s_instance;
}

/**************************************************************************/

KFlashPart::KFlashPart(QWidget *parentWidget, const char *widgetName, QObject *parent,
                       const char *name, const QStringList &args)
    : KParts::ReadOnlyPart(parent, name), m_job(0), m_khtml(0), m_movie(0)
{
//    kdDebug(1432) << "KFlashPart::KFlashPart" << endl;
    setInstance( KFlashFactory::instance() );

    // we have to keep the class name of KParts::KFlashBrowserExtension
    // to let khtml find it
    QObject *ishtml = parentWidget;
    for ( ; ishtml; ishtml=ishtml->parent() ) {
	if ( ishtml->inherits( "KHTMLPart" ) )
	    break;
	if ( ishtml->inherits( "KHTMLView" ) ) {
	    ishtml=static_cast<KHTMLView *>(ishtml)->part();
	    break;
	}
    }
    if ( ishtml )
    {
	m_khtml = static_cast<KHTMLPart *>(ishtml);
	m_extension = m_khtml->browserExtension();
    }
    else
	m_extension = new KFlashBrowserExtension( this );

    // handle arguments
    int settings = PLAYER_LOOP;
    QStringList::ConstIterator it = args.begin();
    for ( ; it != args.end(); it++ ) {

        int equalPos = (*it).find("=");
        if ( equalPos > 0 ) {

            QString name = (*it).left( equalPos ).stripWhiteSpace().lower();
            QString value = (*it).mid( equalPos+1 ).stripWhiteSpace();

	    if ( value.left( 1 ) == "\"" && value.right( 1 ) == "\"" )
		value = value.mid( 1, value.length()-2 );

//            kdDebug(1432) << "name=" << name << " value=" << value << endl;

            if ( !name.isEmpty() ) {
		if ( name == "movie" ) {
		    if ( m_movie )
			*m_movie = value;
		    else
			m_movie = new QString( value );
		}

		value = value.lower();
                if ( name == "quality" ) {
                    if ( value == "high" ||
			 value == "autohigh" ) settings |= PLAYER_QUALITY;
                    if ( value == "low" ) settings &= ~PLAYER_QUALITY;
                } else if ( name == "loop" ) {
                    if ( value == "true" ) settings |= PLAYER_LOOP;
                    if ( value == "false" ) settings &= ~PLAYER_LOOP;
                } else if ( name == "menu" ) {
                    if ( value == "true" ) settings |= PLAYER_MENU;
                    if ( value == "false" ) settings &= ~PLAYER_MENU;
                }
            }
        }
    }

    // create a canvas to insert our widget
    m_widget = new KFlashWidget( parentWidget, widgetName );
    m_widget->setFocusPolicy( QWidget::ClickFocus );
    m_widget->setBackgroundMode( QWidget::NoBackground );
    setWidget( m_widget );
    m_widget->setPart( this );
    m_widget->setSettings( settings );
    QObject::connect( m_widget, SIGNAL( resized(int,int) ),
                      this, SLOT( pluginResized(int,int) ) );
}


KFlashPart::~KFlashPart()
{
//    kdDebug(1432) << "KFlashPart::~KFlashPart" << endl;
    m_widget->setPart( 0 );
    if ( m_job )
	m_job->kill();
    if ( m_movie )
	delete m_movie;
}


bool KFlashPart::openURL(const KURL &url)
{
//    kdDebug(1432) << "-> KFlashPart::openURL " << url.url() << endl;
    QString error;

    // handle dummy URL which appears when only OBJECT but no EMBED exists
    if ( m_movie && !m_movie->isEmpty() )
    {
	m_url = KURL( url, *m_movie );
	delete m_movie;
	m_movie = 0;
    }
    else
	m_url = url;
    QString surl = m_url.url();
    if ( surl.isEmpty() ) {
        kdWarning(1432) << "<- KFlashPart::openURL - false (no url passed to pluginpart)" << endl;
        return false;
    }


    // status messages
    emit setWindowCaption( m_url.prettyURL() );
    emit setStatusBarText( i18n( "Loading KFlash plugin for %1" ).arg( m_url.prettyURL() ) );

    // verify widget
    if ( m_widget ) {
	closeURL();
	widget()->show();
	m_job = KIO::get( m_url, false, false );
	connect( m_job, SIGNAL( result( KIO::Job * ) ),
		 SLOT( slotFinished( KIO::Job * ) ) );
	connect( m_job, SIGNAL( data( KIO::Job*, const QByteArray &)),
		 SLOT( slotData( KIO::Job*, const QByteArray &)));
	emit started( m_job );
        return true;
    }
    if ( error.isEmpty() ) error = i18n("Unable to initialize %1 plugin for %2").arg( "KFlash" ).arg( m_url.url() );
    QLabel *label = new QLabel( error, widget() );
    label->setAlignment( AlignCenter | WordBreak );
    label->resize( widget()->width(), widget()->height() );
    label->show();
    return true; // don't let the browser go to the download page
}


bool KFlashPart::closeURL()
{
//    kdDebug(1432) << "KFlashPart::closeURL" << endl;
    if ( m_job ) {
	m_job->kill();
	m_job = 0;
    }
    return true;
}


void KFlashPart::cbGetUrl( QCString url, QCString target )
{
    kdDebug(1432) << "KFlashPart::cbGetUrl url=" << url
                  << ", target=" << target << endl;

    KURL new_url( this->url(), url );
    if ( new_url.isEmpty() || new_url.isMalformed()
	 || ( new_url.protocol() == "fscommand" ) )
	return;
    if ( new_url.protocol() == "javascript" )
    {
	if ( s_jsEnabled && m_khtml && m_khtml->jScriptEnabled() )
	    m_khtml->executeScript( new_url.prettyURL().replace( QRegExp( "javascript:/*" ), "" ) );
	return;
    }

    KParts::URLArgs args;
    args.frameName = target;
    emit m_extension->openURLRequest( new_url, args );
}


void KFlashPart::cbGetSwf( QCString url, int level )
{
    kdDebug(1432) << "KFlashPart::cbGetSwf url=" << url
                  << ", level=" << level << endl;

// TBD
// Seems to load some other data stream
// Dunno yet how to implement
//    KURL new_url( this->url(), url );
//    emit m_extension->openURLRequest( new_url );
}


void KFlashPart::pluginResized(int w, int h)
{
    kdDebug(1432) << "KFlashPart::pluginResized()" << endl;
    widget()->resize( w, h );
}


void KFlashPart::slotData( KIO::Job* /*kio_job*/, const QByteArray &data )
{
    if ( data.size() == 0 )
	return;

    if ( !m_widget->addData( data ) ) {
	closeURL();
	emit canceled( i18n("Invalid data.") );
    }
}

void KFlashPart::slotFinished( KIO::Job * job )
{
    if ( job->error() )
	emit canceled( job->errorString() );
    else
	emit completed();
    m_job = 0;
}



KFlashWidget::KFlashWidget(QWidget *parent, const char *name)
      : QWidget(parent,name), m_image(0), m_handle(0), m_delay(0), m_settings(0)
{
    reCompute();
    m_handle = FlashNew();
    QObject::connect( &m_timer, SIGNAL( timeout() ),
		      this, SLOT( slotTimeout() ) );
}


KFlashWidget::~KFlashWidget()
{
    m_timer.stop();
    if ( m_handle )
        FlashClose( m_handle );
    if ( m_image )
        delete m_image;
}


void KFlashWidget::initFlash()
{
    if ( m_delay || !m_handle )
	return;
//    kdDebug(1432) << "KFlashWidget::initFlash()" << endl;
    FlashGetInfo( m_handle, &m_info );
    m_delay = 1000/m_info.frameRate;

    FlashSettings( m_handle, m_settings );
    // graphic init here
    FlashGraphicInit( m_handle, &m_disp );
    if ( s_soundEnabled )
	FlashSoundInit( m_handle, "/dev/dsp" );
    FlashSetGetUrlMethod( m_handle, getUrl, m_part );
    FlashSetGetSwfMethod( m_handle, getSwf, m_part );
    FlashSetCursorOnOffMethod( m_handle, cursorOnOff, this );
    m_timer.start( m_delay );
}


bool KFlashWidget::addData( const QByteArray &data, int level )
{
    if ( data.size() ) {
	int tmp = m_data.size();
	if ( tmp ) {
	    m_data.resize( tmp + data.size() );
	    memcpy( m_data.data() + tmp, data.data(), data.size() );
	}
	else
	    m_data = data;
    }

    int status = FlashParse( m_handle, level, m_data.data(), m_data.size() );
    if ( ( status == FLASH_PARSE_ERROR ) || ( status & FLASH_PARSE_OOM ) )
	return false;

    if ( status & FLASH_PARSE_START )
	initFlash();

    return true;
}

void KFlashWidget::reCompute()
{
    if ( !m_image ) {
	int bitdepth = QPixmap::defaultDepth();
	switch ( bitdepth ) {
	    // Flashlib only supports 16, 24, 32
	    // QImage   only supports  1,  8, 32
//	    case 16:
//	    case 24:
	    case 32:
		break;
	    default:
		bitdepth=32;
	}
	m_image=new QImage( width(), height(), bitdepth );
	if ( m_image ) {
	    m_image->fill( 0 );
	}
    }
    if ( !m_image ) {
	kdWarning(1432) << "KFlashPart::reCompute(): Failed to create QImage!!!" << endl;
	return;
    }
    memset( &m_disp, 0, sizeof(m_disp) );
    m_disp.width=m_image->width();
    m_disp.height=m_image->height();
    m_disp.bpl=m_image->bytesPerLine();
    m_disp.depth=m_image->depth();
    m_disp.bpp=m_image->depth()/8;
    m_disp.pixels=m_image->bits();
}


void KFlashWidget::resizeEvent( QResizeEvent *ev )
{
    QWidget::resizeEvent( ev );
    if ( m_image )
	*m_image = m_image->smoothScale( width(), height() );
    reCompute();
    if ( m_delay )
	FlashGraphicInit( m_handle, &m_disp );
    emit resized( width(), height() );
}


void KFlashWidget::paintEvent ( QPaintEvent * /*ev*/ )
{
    if ( m_image ) {
        QPainter p( this );
        p.drawImage( 0, 0, *m_image );
    }
}


void KFlashWidget::execFlash( long flag, FlashEvent *fe )
{
    if ( !m_handle )
	return;

    struct timeval tm;
    FlashExec(m_handle, flag, fe, &tm);
    if ( m_disp.flash_refresh )
        update(); // Or repaint() ?
}

bool KFlashWidget::event( QEvent *ev )
{
    if ( !m_handle )
	return QWidget::event( ev );

    FlashEvent fe;
    fe.type = FeNone;
    switch ( ev->type() ) {
	case QEvent::MouseMove:
	    fe.type = FeMouseMove;
	    break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    fe.type = FeButtonPress;
	    break;
	case QEvent::MouseButtonRelease:
	    fe.type = FeButtonRelease;
	    break;
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	    fe.type = (ev->type() == QEvent::KeyPress) ? FeKeyPress : FeKeyRelease;
	    switch ( ((QKeyEvent*)ev)->key() ) {
		case Key_Up:
		    fe.key = FeKeyUp;
		    break;
		case Key_Down:
		    fe.key = FeKeyDown;
		    break;
		case Key_Left:
		    fe.key = FeKeyLeft;
		    break;
		case Key_Right:
		    fe.key = FeKeyRight;
		    break;
		case Key_Return:
		case Key_Enter:
		    fe.key = FeKeyEnter;
		    break;
		default:
		    fe.type = FeNone;
	    }
	    break;
	default:
	    break;
    }
    switch ( ev->type() ) {
	case QEvent::MouseMove:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	    fe.x = ((QMouseEvent*)ev)->x();
	    fe.y = ((QMouseEvent*)ev)->y();
	default:
	    break;
    }
    if ( fe.type != FeNone ) {
	execFlash( FLASH_EVENT, &fe );
	return true;
    }

    return QWidget::event( ev );
}


void KFlashWidget::slotTimeout()
{
    execFlash( FLASH_WAKEUP );
}

void KFlashWidget::cbCursorOnOff( int on )
{
    kdDebug(1432) << "KFlashWidget::cbSetCursorOnOff on=" << on << endl;

    if ( on ) unsetCursor();
    else setCursor( Qt::blankCursor );
}


extern "C" void* init_libkflash()
{
#if defined(KONQ_EMBEDDED)
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "KFlashPart" ) );
    if ( !config->readBoolEntry( "GlobalEnabled", true ) )
	return 0;
    s_soundEnabled = config->readBoolEntry( "SoundEnabled", false );
    s_jsEnabled = config->readBoolEntry( "JsEnabled", false );
#endif
    return new KFlashFactory;
}

#if defined(KONQ_EMBEDDED)
#include <kio/mimehandler.h>

extern "C" void konqe_register_libkflash()
{
    KFlashFactory *factory = (KFlashFactory *) init_libkflash();
    if ( factory ) {
	QStringList types;
	types << "application/x-shockwave-flash" << ".swf";
	types << "application/x-futuresplash" << ".spl";
	KIO::MimeHandler::AddHandler( new KIO::MimePart( types, factory ) );
    }
}

#endif
