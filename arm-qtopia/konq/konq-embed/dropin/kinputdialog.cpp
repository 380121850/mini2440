
#include "kinputdialog.h"

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include <klocale.h>

#ifdef QT_NO_INPUTDIALOG

QString QInputDialog::getText( const QString &caption, const QString &labelText,
                               const QString &text )
{
    QDialog *dialog = new QDialog( 0, 0, true );
    dialog->setCaption( caption );

    QVBoxLayout *layout = new QVBoxLayout( dialog );
    layout->setSpacing( 6 );
    layout->setMargin( 11 );
  
    QLabel *label = new QLabel( labelText, dialog );
    layout->addWidget( label );

    QLineEdit *lineEdit = new QLineEdit( text, dialog );
    layout->addWidget( lineEdit );

// should be !defined, enabled only for testing
#if defined(_QT_QPE_)
    QHBoxLayout *buttonLayout = new QHBoxLayout( layout );
    
    buttonLayout->addStretch( 1 );

    QPushButton *ok = new QPushButton( i18n( "&OK" ), dialog );
    buttonLayout->addWidget( ok );
    QObject::connect( ok, SIGNAL( clicked() ), dialog, SLOT( accept() ) );

    QPushButton *cancel = new QPushButton( i18n( "&Cancel" ), dialog );
    buttonLayout->addWidget( cancel );
    QObject::connect( cancel, SIGNAL( clicked() ), dialog, SLOT( reject() ) );
   
#endif
  
    QString res;

    if ( dialog->exec() == QDialog::Accepted )
        res = lineEdit->text();

#warning the kjs code should check for ok!!

    delete dialog;

    return res;
}

#endif

