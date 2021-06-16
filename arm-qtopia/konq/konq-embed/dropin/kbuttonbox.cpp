
#include "kbuttonbox.h"

KButtonBox::KButtonBox( QWidget *parent, Orientation _orientation )
    : QWidget( parent )
{
    if ( _orientation == Horizontal )
        m_layout = new QHBoxLayout( this );
    else
        m_layout = new QVBoxLayout( this );


}

QPushButton *KButtonBox::addButton(const QString& text, QObject * receiver, const char * slot, bool noexpand )
{
    QPushButton *pb = new QPushButton( text, this );
    pb->adjustSize();

    m_layout->addWidget( pb );

    connect( pb, SIGNAL( clicked() ), receiver, slot );

    return pb;
}
