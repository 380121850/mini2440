#ifndef __kprinter_h__
#define __kprinter_h__

#include <config.h>

#include <qprinter.h>

#if defined(ENABLE_PRINTING)

#include <qprintdialog.h>

class KPrinter : public QPrinter
{
public:    
    bool setup( QWidget *parent = 0 )
	{ return QPrintDialog::getPrinterSetup( this ); }
};
#else
typedef QPrinter KPrinter;
#endif

#endif

