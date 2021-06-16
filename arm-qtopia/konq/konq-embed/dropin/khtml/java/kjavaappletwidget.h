#ifndef __kjavaappletwidget_h__
#define __kjavaappletwidget_h__

#include <qwidget.h>

class KJavaAppletContext;
class KJavaApplet;

class KJavaAppletWidget : public QWidget
{
public:
    KJavaAppletWidget( KJavaAppletContext *, QWidget *parent, const char *name = 0 )
        : QWidget( parent, name )
    {}

    KJavaApplet *applet() const { return 0; }

    void showApplet() {}
};

class KJavaApplet
{
public:
    KJavaApplet() {}

    void create() {};

    void setParameter( const QString &, const QString & ) {}

    void setBaseURL( const QString & ) {}

    void setAppletClass( const QString & ) {}

    void setCodeBase( const QString & ) {}

    void setAppletName( const QString & ) {}

    void setJARFile( const QString & ) {}

    void setArchives( const QString & ) {}
};

#endif
