#ifndef __kcombobox_h__
#define __kcombobox_h__

#include <qcombobox.h>

class KComboBox : public QComboBox
{
    Q_OBJECT
public:
    KComboBox( bool rw, QWidget *parent = 0, const char *name = 0 )
        : QComboBox( rw, parent, name ) {}
};

#endif

