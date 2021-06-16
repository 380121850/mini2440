/****************************************************************************
** Form interface generated from reading ui file '../../../../konq-embed/add-ons/pppdialer/dialer.ui'
**
** Created: Mon Jan 13 20:08:16 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DIALERBASE_H
#define DIALERBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QFrame;
class QLabel;
class QPushButton;
class QWidgetStack;

class DialerBase : public QDialog
{ 
    Q_OBJECT

public:
    DialerBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DialerBase();

    QWidgetStack* m_switcher;
    QFrame* m_infos;
    QLabel* m_label1;
    QLabel* m_label2;
    QComboBox* m_which;
    QPushButton* m_action;
    QPushButton* m_close;

protected:
    QVBoxLayout* DialerBaseLayout;
    QVBoxLayout* m_infosLayout;
    QHBoxLayout* Layout1;
};

#endif // DIALERBASE_H
