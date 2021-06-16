#ifndef __kxmlguiclient_h__
#define __kxmlguiclient_h__

#include <qstring.h>
#include <qlist.h>
#include <qdom.h>

#include <kaction.h>

class KXMLGUIClient
{
public:
    KXMLGUIClient() {}
    virtual ~KXMLGUIClient() {}

    void setXMLFile( const QString & ) {}

    KActionCollection *actionCollection() { return &m_collection; }

    void plugActionList( const QString &, const QList<KAction> & ) {}
    void unplugActionList( const QString & ) {}

    void setInstance( KInstance * ) {}

    void setXML( const QString & ) {}

private:
    KActionCollection m_collection;
};

#endif
