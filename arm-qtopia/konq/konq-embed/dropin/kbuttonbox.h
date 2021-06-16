#ifndef __kbuttonbox_h__
#define __kbuttonbox_h__

#include <qwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>

class KButtonBox : public QWidget
{
public:
    KButtonBox( QWidget *parent, Orientation _orientation = Horizontal );

    QPushButton *addButton(const QString& text, QObject * receiver, const char * slot, bool noexpand = FALSE);

    void layout() {}

    void addStretch() { m_layout->addStretch(); }

private:
    QBoxLayout *m_layout;
};

#endif
