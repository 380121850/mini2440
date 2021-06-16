
#include "kfiledialog.h"

#include <qlayout.h>

QString KFileDialog::getOpenFileName( const QString &dir,
                                      const QString &filter,
                                      QWidget *parent, const QString &caption )
{
#if defined(_QT_QPE_)
    QPEFileSelectorWrapper *qpefd = new QPEFileSelectorWrapper( parent );
    qpefd->setCaption( caption );
    qpefd->showMaximized();

    QString result;

    if ( qpefd->exec() == QDialog::Accepted )
        result = qpefd->selectedPath(); 

    delete qpefd;

    return result;
#elif defined(QT_NO_FILEDIALOG)
    return QString::null; // sorry bud :)
#else
    return QFileDialog::getOpenFileName( dir, filter, parent, 0, caption );
#endif
}

#if defined(ENABLE_READWRITE)
QString KFileDialog::getSaveFileName( const QString &dir,
                                      const QString &filter,
                                      QWidget *parent, const QString &caption )
{
#if defined(_QT_QPE_)
    QPEFileSelectorWrapper *qpefd = new QPEFileSelectorWrapper( parent );
    qpefd->setCaption( caption );
    qpefd->showMaximized();

    QString result;

    if ( qpefd->exec() == QDialog::Accepted )
        result = qpefd->selectedPath(); 

    delete qpefd;

    return result;
#elif defined(QT_NO_FILEDIALOG)
    return QString::null; // sorry bud :)
#else
    return QFileDialog::getSaveFileName( dir, filter, parent, 0, caption );
#endif
}
#endif

#if defined(_QT_QPE_) || defined(Q_WS_WIN32)

QPEFileSelectorWrapper::QPEFileSelectorWrapper( QWidget *parent, const char *name )
    : QDialog( parent, name, true )
{
#if defined(_QT_QPE_)
    QVBoxLayout *layout = new QVBoxLayout( this );

    FileSelector *fs = new FileSelector( QString::null, this, "fs" );
    layout->addWidget( fs );

    fs->setNewVisible( false );

    connect( fs, SIGNAL( fileSelected( const DocLnk & ) ),
             this, SLOT( fileSelected( const DocLnk & ) ) );
    connect( fs, SIGNAL( closeMe() ),
             this, SLOT( accept() ) );
#endif
}

QPEFileSelectorWrapper::~QPEFileSelectorWrapper()
{
}

void QPEFileSelectorWrapper::fileSelected( const DocLnk &doc )
{
    m_path = doc.file();
}

#include "kfiledialog.moc"
#endif
