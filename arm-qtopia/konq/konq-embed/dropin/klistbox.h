#ifndef __klistbox_h__
#define __klistbox_h__

#include <qlistbox.h>

class KListBox : public QListBox
{
    Q_OBJECT
public:
    KListBox( QWidget *parent = 0, const char *name = 0 )
        : QListBox( parent, name ) {}
};

#endif
