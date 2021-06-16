
#include "kmessagebox.h"

#include <qtimer.h>
#include <klocale.h>

int KMessageBox::warningYesNoCancel( QWidget *parent,
                                const QString &text,
                                const QString &caption,
                                const QString &buttonYes,
                                const QString &buttonNo,
                                bool cancelButton,
                                bool okButton )
{
    QMessageBox mb( caption, text, QMessageBox::NoIcon,
                    okButton ? QMessageBox::Ok : QMessageBox::Yes,
                    okButton ? QMessageBox::NoButton : QMessageBox::No,
                    cancelButton ? QMessageBox::Cancel : QMessageBox::NoButton,
                    parent );

    if ( !buttonYes.isEmpty() )
        mb.setButtonText( QMessageBox::Yes, buttonYes );

    if ( !buttonNo.isEmpty() )
        mb.setButtonText( QMessageBox::No, buttonNo );

    switch ( mb.exec() )
    {
	case QMessageBox::Yes:
	    return Yes;
	case QMessageBox::No:
	    return No;
	default:
	    return Cancel;
    }
}

int KMessageBox::warningContinueCancel( QWidget *parent,
                                const QString &text,
                                const QString &caption,
                                const QString &buttonContinue )
{
    return warningYesNoCancel( parent, text, caption, buttonContinue, i18n( "Cancel" ),
			       false ) == Yes ? Continue : Cancel;
}

MessageBoxQueuer *MessageBoxQueuer::s_self = 0;

MessageBoxQueuer *MessageBoxQueuer::self()
{
    if ( !s_self )
        s_self = new MessageBoxQueuer;
    return s_self;
}

void MessageBoxQueuer::queue( QWidget *parent, int type, const QString &text,
                              const QString &caption )
{
    Entry e;
    e.parent = parent;
    e.type = type;
    e.text = text;
    e.caption = caption;
    self()->m_queue.append( e );
    QTimer::singleShot( 0, self(), SLOT( dequeue() ) );
}

void MessageBoxQueuer::dequeue()
{
    if ( m_lock )
        return;

    Entry e;
    do
    {
        if ( m_queue.isEmpty() )
            return;

        e = m_queue.first();
        m_queue.remove( m_queue.begin() );
    } while ( !e.parent );

    m_lock = true;
    switch ( e.type )
    {
        case KMessageBox::QuestionYesNo:
            KMessageBox::questionYesNo( e.parent, e.text, e.caption );
            break;
        case KMessageBox::WarningYesNo:
            KMessageBox::warningYesNo( e.parent, e.text, e.caption );
            break;
        case KMessageBox::WarningContinueCancel:
            KMessageBox::warningContinueCancel( e.parent, e.text, e.caption );
            break;
        case KMessageBox::WarningYesNoCancel:
            KMessageBox::warningYesNoCancel( e.parent, e.text, e.caption );
            break;
        case KMessageBox::Information:
            KMessageBox::information( e.parent, e.text, e.caption );
            break;
        case KMessageBox::Sorry:
            KMessageBox::sorry( e.parent, e.text, e.caption );
            break;
        case KMessageBox::Error:
            KMessageBox::error( e.parent, e.text, e.caption );
            break;
    }
    m_lock = false;

    if ( !m_queue.isEmpty() )
        QTimer::singleShot( 20, this, SLOT( dequeue() ) );

    delete this;
    s_self = 0;
}

#include "kmessagebox.moc"
