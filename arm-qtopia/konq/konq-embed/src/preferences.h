/****************************************************************************
** Form interface generated from reading ui file '../../../../src/kdenox/konq-embed/src/preferences.ui'
**
** Created: Mon Mar 11 10:36:21 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PREFERENCESBASE_H
#define PREFERENCESBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QWidget;

class PreferencesBase : public QDialog
{ 
    Q_OBJECT

public:
    PreferencesBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PreferencesBase();

    QTabWidget* tabWidget;
    QWidget* Widget4;
    QLabel* TextLabel5;
    QLineEdit* homePage;
    QPushButton* currentPage;
    QFrame* Line2;
    QWidget* Widget5;
    QLabel* TextLabel3;
    QFrame* Line4;
    QCheckBox* useProxy;
    QLineEdit* proxyServer;
    QComboBox* cookiePolicy;
    QLabel* TextLabel6;
    QFrame* Line5;
    QCheckBox* enableJavaScript;
    QWidget* tab;
    QSpinBox* minimumFontSize;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QComboBox* defaultFont;
    QLabel* TextLabel1_2;
    QSpinBox* fixedFontSize;
    QFrame* Line4_2;
    QCheckBox* forceFixedFontSize;
    QPushButton* defaultsButton;
    QPushButton* okButton;
    QPushButton* cancelButton;

public slots:
    virtual void defaults();
    virtual void makeCurrentSiteHomePage();

protected:
    QVBoxLayout* PreferencesBaseLayout;
    QGridLayout* Widget4Layout;
    QGridLayout* Widget5Layout;
    QGridLayout* tabLayout;
    QHBoxLayout* Layout2;
};

#endif // PREFERENCESBASE_H
