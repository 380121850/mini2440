/* This file is part of the KDE Project
   Copyright (c) 2002 Paul Chitescu <paulc-devel@null.ro>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "ksimplesound.h"

#include <kinstance.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qsound.h>

KSimpleSound::KSimpleSound( QWidget *parentWidget, const char *widgetName,
			    QObject *parent, const char *name,
			    const QStringList &args )
    : KParts::ReadOnlyPart( parent, name )
{
    setInstance( SimpleSoundFactory::instance() );

    m_autoplay = true;
    m_loop = false;
    QStringList::ConstIterator it = args.begin();
    for ( ; it != args.end(); it++ )
    {
	int equalPos = (*it).find( "=" );
	if ( equalPos > 0 )
	{
	    QString name = (*it).left( equalPos ).stripWhiteSpace().lower();
	    QString value = (*it).mid( equalPos+1 ).stripWhiteSpace();

	    if ( value.left( 1 ) == "\"" && value.right( 1 ) == "\"" )
		value = value.mid( 1, value.length()-2 );

	    value = value.lower();

	    if ( name == "autostart" && value == "false" )
		m_autoplay = false;
	    else if ( name == "loop" && value == "true" )
		m_loop = true;
	}
    }

    QPushButton *btn = new QPushButton( i18n( "Play" ), parentWidget, widgetName );
    connect( btn, SIGNAL( clicked() ),
	     this, SLOT( play() ) );
    btn->setFlat( true );
    setWidget( btn );
    setXMLFile( "ksimplesound.rc" );
    KStdAction::open( this, SLOT( fileOpen() ), actionCollection() );
    new KAction( btn->text(), "1rightarrow.png", 0, this, SLOT( play() ),
		 actionCollection(), "player_play" );
#if defined(KONQ_EMBEDDED)
    new QObject( actionCollection(), "player_bar" );
#endif
}

KSimpleSound::~KSimpleSound()
{
}

void KSimpleSound::play()
{
    if ( !m_file.isEmpty() )
	QSound::play( m_file );
}

bool KSimpleSound::openFile()
{
    if ( !m_file.isEmpty() )
    {
	if ( m_autoplay )
	    play();
	return true;
    }
    return false;
}

void KSimpleSound::fileOpen()
{
    QString file_name = KFileDialog::getOpenFileName( m_file );

    if ( !file_name.isEmpty() )
	openURL( file_name );
}

KInstance* s_instance = 0L;
KAboutData* s_about = 0L;

SimpleSoundFactory::SimpleSoundFactory()
    : KParts::Factory()
{
}

SimpleSoundFactory::~SimpleSoundFactory()
{
    delete s_instance;
    delete s_about;
	
    s_instance = 0L;
}

KParts::Part* SimpleSoundFactory::createPartObject(
		    QWidget *parentWidget, const char *widgetName,
		    QObject *parent, const char *name,
		    const char * /*classname*/, const QStringList &args )
{
    if ( QSound::available() )
	return new KSimpleSound( parentWidget, widgetName, parent, name, args );

    return 0;
}

KInstance* SimpleSoundFactory::instance()
{
    if ( !s_instance )
    {
	s_about = new KAboutData( "ksimplesound", I18N_NOOP( "KSimpleSound" ), "0.1" );
	s_about->addAuthor( "Paul Chitescu", 0, "paulc-devel@null.ro" );
	s_instance = new KInstance( s_about );
    }
    return s_instance;
}

extern "C" void *init_libksimplesound()
{
    return new SimpleSoundFactory;
}

#if defined(KONQ_EMBEDDED)

#include <kdebug.h>

#if defined(ENABLE_JS_HOSTEXTEND)

#include <jsextension.h>
#include <kjs/object.h>
#include <kjs/types.h>
#include <kjs/function.h>

class JsSoundPlay : public KJS::InternalFunctionImp
{
public:
    JsSoundPlay( KSimpleSound *sound ) : m_sound( sound ) {}
    virtual ~JsSoundPlay()
	{ kdDebug()<< "JsSoundPlay::~JsSoundPlay()" <<endl; }
    virtual KJS::Completion execute( const KJS::List &args );

protected:
    KSimpleSound *m_sound;
};

KJS::Completion JsSoundPlay::execute( const KJS::List & )
{
    m_sound->play();
    return KJS::Completion( KJS::ReturnValue, KJS::Undefined() );
}

static void JsSoundExtension( KParts::ReadOnlyPart *part, const KURL &, const QString &, KJS::KJSO &hostobj )
{
    if ( !part->inherits( "KSimpleSound" ) )
	return;

    kdDebug()<< "Connecting JsSoundPlay for "<< part->name() << " in "
	     << hostobj.toString().value().qstring() <<endl;

    hostobj.put( "play", KJS::KJSO( new JsSoundPlay( static_cast<KSimpleSound *>( part ) ) ) );
}

#endif // ENABLE_JS_HOSTEXTEND

#include <kconfig.h>
#include <kio/mimehandler.h>

extern "C" void konqe_register_libksimplesound()
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "KSimpleSound" ) );
    if ( config->readBoolEntry( "GlobalEnabled", true ) ) {
	if ( !QSound::available() )
	{
	    kdWarning()<< "QSound support is not available!" <<endl;
	    return;
	}
	QStringList types;
	types << "audio/wav" << "audio/x-wav" << ".wav";
	KIO::MimeHandler::AddHandler( new KIO::MimePart( types, new SimpleSoundFactory ) );
#if defined(ENABLE_JS_HOSTEXTEND)
	if ( config->readBoolEntry( "JsEnabled", true ) )
	    KJS::addJsExtension( &JsSoundExtension ); 
#endif
    }
}
#endif // KONQ_EMBEDDED

#include "ksimplesound.moc"
