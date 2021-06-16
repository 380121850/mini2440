
#include <kinstance.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfiledialog.h>

#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmultilineedit.h>

#include "kplaintext_part.h"

#include <kmessagebox.h>

KPlainTextPart::KPlainTextPart( QWidget *parentWidget, const char *widgetName,
                                  QObject *parent, const char *name )
    : KParts::ReadWritePart( parent, name )
{
    // we need an instance
    setInstance( KPlainTextPartFactory::instance() );

    m_widget = new QMultiLineEdit( parentWidget, widgetName );

    setWidget( m_widget );

    setXMLFile( "kplaintextpart.rc" );

    setupActions();

    // we are read-write by default
    setReadWrite( true );

    // we are not modified since we haven't done anything yet
    setModified( false );
}

KPlainTextPart::~KPlainTextPart()
{
}

void KPlainTextPart::setupActions()
{
    KStdAction::open( this, SLOT(fileOpen()), actionCollection() );
    KStdAction::saveAs( this, SLOT(fileSaveAs()), actionCollection() );
    KStdAction::save( this, SLOT(save()), actionCollection() );
    KAction * act;
    act = KStdAction::undo( m_widget, SLOT(undo()), actionCollection() );
    act->setEnabled( false );
    connect( m_widget, SIGNAL(undoAvailable(bool)), act, SLOT(setEnabled(bool)) );
    act = KStdAction::redo( m_widget, SLOT(redo()), actionCollection() );
    act->setEnabled( false );
    connect( m_widget, SIGNAL(redoAvailable(bool)), act, SLOT(setEnabled(bool)) );
    act = KStdAction::cut( m_widget, SLOT(cut()), actionCollection() );
    act->setEnabled( false );
    connect( m_widget, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)) );
    act->setEnabled( false );
    act = KStdAction::copy( m_widget, SLOT(copy()), actionCollection() );
    act->setEnabled( false );
    connect( m_widget, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)) );
    KStdAction::paste( m_widget, SLOT(paste()), actionCollection() );
    KStdAction::selectAll( m_widget, SLOT(selectAll()), actionCollection() );
#if defined(KONQ_EMBEDDED)
    new QObject( actionCollection(), "edit_bar" );
    new QObject( actionCollection(), KStdAction::stdName( KStdAction::ShowStatusbar ) );
#endif
}

void KPlainTextPart::setReadWrite( bool rw )
{
    // notify your internal widget of the read-write state
    m_widget->setReadOnly( !rw );
    if ( rw )
        connect( m_widget, SIGNAL( textChanged() ),
                 this, SLOT( setModified() ) );
    else
        disconnect( m_widget, SIGNAL( textChanged() ),
                    this, SLOT( setModified() ) );

    ReadWritePart::setReadWrite(rw);
}

void KPlainTextPart::setModified( bool modified )
{
    KAction *save = actionCollection()->action( KStdAction::stdName( KStdAction::Save ) );
    if ( save )
        save->setEnabled( modified );

    // in any event, we want our parent to do it's thing
    ReadWritePart::setModified( modified );
}

bool KPlainTextPart::openFile()
{
    // fname is always local so we can use QFile on it
    QFile file( m_file );
    if ( !file.open( IO_ReadOnly ) )
        return false;

    // our example widget is text-based, so we use QTextStream instead
    // of a raw QDataStream
    QTextStream stream( &file );
    QString str;
    while ( !stream.eof() )
        str += stream.readLine() + "\n";

    file.close();

    // now that we have the entire file, display it
    m_widget->setText( str );
    m_widget->setCursorPosition( 0, 0 );

    setModified( false );
    setCaption();

    return true;
}

bool KPlainTextPart::saveFile()
{
    // if we aren't read-write, return immediately
    if ( !isReadWrite() )
        return false;

    // m_file is always local, so we use QFile
    QFile file( m_file );
    if ( !file.open( IO_WriteOnly ) )
        return false;

    // use QTextStream to dump the text to the file
    QTextStream stream( &file );
    stream << m_widget->text();

    file.close();

    return true;
}

void KPlainTextPart::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    QString file_name = KFileDialog::getOpenFileName( suggestedName() );

    if ( !file_name.isEmpty() )
        openURL( file_name );
}

void KPlainTextPart::fileSaveAs()
{
    // this slot is called whenever the File->Save As menu is selected,
    QString file_name = KFileDialog::getSaveFileName( suggestedName() );
    if ( !file_name.isEmpty() && saveAs( file_name ) )
	setCaption();
}

const QString KPlainTextPart::suggestedName()
{
    return m_url.isLocalFile() ? m_url.path() :
	QString::fromLatin1( "%1/%2" ).arg( QDir::currentDirPath() )
				      .arg( m_url.fileName() );
}

void KPlainTextPart::setCaption()
{
    emit setWindowCaption( i18n( "Text Editor: %1" ).arg( m_url.prettyURL() ) );
}


KInstance*  KPlainTextPartFactory::s_instance = 0L;
KAboutData* KPlainTextPartFactory::s_about = 0L;

KPlainTextPartFactory::KPlainTextPartFactory()
    : KParts::Factory()
{
}

KPlainTextPartFactory::~KPlainTextPartFactory()
{
    delete s_instance;
    delete s_about;

    s_instance = 0L;
}

KParts::Part* KPlainTextPartFactory::createPartObject( QWidget *parentWidget, const char *widgetName,
                                                       QObject *parent, const char *name,
                                                       const char *classname, const QStringList & /*args*/ )
{
    // Create an instance of our Part
    KPlainTextPart* obj = new KPlainTextPart( parentWidget, widgetName, parent, name );

    // See if we are to be read-write or not
    if ( QCString( classname ) == "KParts::ReadOnlyPart" )
        obj->setReadWrite( false );

    return obj;
}

KInstance* KPlainTextPartFactory::instance()
{
    if ( !s_instance )
    {
        // the non-i18n name here must be the same as the directory in
        // which the part's rc file is installed ('partrcdir' in the
        // Makefile)
        s_about = new KAboutData( "kplaintextpart", I18N_NOOP("KPlainTextPart"), "0.1" );
        s_about->addAuthor( "Paul Chitescu", 0, "paulc-devel@null.ro" );
        s_instance = new KInstance( s_about );
    }
    return s_instance;
}

extern "C"
{
    void* init_libkplaintextpart()
    {
        return new KPlainTextPartFactory;
    }
};

#if defined(KONQ_EMBEDDED)
#include <kconfig.h>
#include <kio/mimehandler.h>

extern "C" void konqe_register_libkplaintextpart()
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver( config, QString::fromLatin1( "KPlainTextPart" ) );
    if ( config->readBoolEntry( "GlobalEnabled", true ) ) {
        QStringList types;
        types << "text/plain" << ".txt";
        types << "text/script" << "text/english";
        KIO::MimeHandler::AddHandler( new KIO::MimePart( types, new KPlainTextPartFactory, true ) );
    }
}

#endif

#include "kplaintext_part.moc"
