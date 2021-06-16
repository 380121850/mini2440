#ifndef __klineedit_h__
#define __klineedit_h__

#include <qlineedit.h>

#include <kcompletionbox.h>

class KCompletion
{
public:
    void setItems( const QStringList & ) {} 
};

class KLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    KLineEdit( QWidget *parent = 0, const char *name = 0 )
        : QLineEdit( parent, name ) {}

    KCompletion *completionObject() { return &m_completionObj; }

    KCompletionBox *completionBox( bool ) { return &m_completionBox; }

    void setContextMenuEnabled( bool ) {}

private:
    KCompletion m_completionObj;
    KCompletionBox m_completionBox;
};

#endif

