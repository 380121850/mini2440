#ifndef __kdummyprinter_h__
#define __kdummyprinter_h__

#include <qfeatures.h>

#ifdef QT_NO_PRINTER

#include <qstring.h>
#include <qpaintdevice.h>

class QWidget;

class QPrinter : public QPaintDevice
{
public:
    QPrinter() : QPaintDevice( 0 ) {}

    bool setup( QWidget * ) { return false; }

    void setFullPage( bool ) {}
    void setCreator( const QString & ) {}
    void setDocName( const QString & ) {}
    QSize margins() const { return QSize(); }
    void newPage() {}
};

#endif

#endif
