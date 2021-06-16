#ifndef __klibloader_h__
#define __klibloader_h__

#include <qobject.h>
#include <qasciidict.h>
#include <qcstring.h>

class KLibFactory;

class KLibrary
{
public:
    KLibrary( const QCString &name );
    virtual ~KLibrary();

    QString name() const { return QString::fromLatin1( m_name ); }

    void registerSymbol( const char *key, void *symbol );
    virtual void *symbol( const char *name );

    KLibFactory *factory();

private:
    QAsciiDict<void> m_symbols;
    QCString m_name;
    KLibFactory *m_factory;
};

class KLibFactory : public QObject
{
    Q_OBJECT
public:
    KLibFactory( QObject *parent = 0, const char *name = 0 )
        : QObject( parent, name ) {}

    QObject *create( QObject *parent, const char *name, const char *classname, const QStringList &args = QStringList() )
        { return createObject( parent, name, classname, args ); }

protected:
    virtual QObject *createObject( QObject *, const char *, const char *, const QStringList & ) = 0;

signals:
    void objectCreated( QObject * );
};

class KLibLoader
{
public:
    enum LibraryOpeningPolicy { TryDlopen, CreateStaticWrapper };

    KLibLoader() { s_self = this; }

    KLibrary *library( const char *name, LibraryOpeningPolicy policy = TryDlopen );

    KLibrary *globalLibrary( const char *name );

    KLibFactory *factory( const char *name, LibraryOpeningPolicy policy = TryDlopen )
        { return library( name, policy )->factory(); }

    void unloadLibrary( const char * ) {}

    static KLibLoader *self();

private:
    QCString findLibrary( const QCString &name );

    static KLibLoader *s_self;
    QAsciiDict<KLibrary> m_libs;
};

#endif
