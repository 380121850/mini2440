#ifndef __kconfig_h__
#define __kconfig_h__

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qcolor.h>
#include <qdatetime.h>

class KConfigBackend
{
public:
    virtual ~KConfigBackend();

    virtual void load() = 0;

    virtual void save() = 0;

    virtual QString group() = 0;

    virtual void setGroup( const QString &group ) = 0;

    virtual QStringList groupList() const = 0;

    virtual bool hasGroup( const QString &group ) const = 0;

    virtual bool hasKey( const QString &key ) = 0;

    virtual QMap<QString, QString> entryMap( const QString &group ) = 0;

    virtual bool lookup( const QString &key, QString &value ) = 0;

    virtual void put( const QString &key, const QString &value ) = 0;

    virtual void deleteGroup( const QString &group ) = 0;
};

class KConfigFileBackend : public KConfigBackend
{
public:
    KConfigFileBackend( const QString &fileName );
    virtual ~KConfigFileBackend();

    virtual void load();

    virtual void save();

    virtual QString group();

    virtual void setGroup( const QString &group );

    virtual QStringList groupList() const;

    virtual bool hasGroup( const QString &group ) const;

    virtual bool hasKey( const QString &key );

    virtual QMap<QString, QString> entryMap( const QString &group );

    virtual bool lookup( const QString &key, QString &value );

    virtual void put( const QString &key, const QString &value );

    virtual void deleteGroup( const QString &group );
private:
    void parse( const QString &fileName );

    QString m_fileName;

    // maps field -> value
    typedef QMap<QString,QString> EntryMap;
    // maps group -> entry-map
    typedef QMap<QString,EntryMap> GroupMap;

    GroupMap m_data;
    GroupMap::Iterator m_current;
};

class KConfig
{
public:
    KConfig( const QString &fileName, bool readOnly = false, bool = false );
    // note that this constructor transfers the ownership of the backend,
    // meaning the kconfig destructor deletes the passed backend object.
    KConfig( KConfigBackend *backEnd, bool readOnly );
    ~KConfig();

    QString group() { return m_backend->group(); }
    void setGroup( const QString &group ) { m_backend->setGroup( group ); }

    void writeEntry( const QString &key, const QString &value );
    void writeEntry( const QString &key, int value );
    void writeEntry( const QString &key, bool value );
    void writeEntry( const QString &key, const QStringList &value, QChar sep = ',' );
    void writeEntry( const QString &key, const QDateTime &dateTime );

    bool hasGroup( const QString &group ) const { return m_backend->hasGroup( group ); }
    bool hasKey( const QString &key ) { return m_backend->hasKey( key ); }

    QStringList groupList() const { return m_backend->groupList(); }

    QString readEntry( const QString &key, const QString &defaultValue = QString::null );
    int readNumEntry( const QString &key, int defaultValue = 0 );
    uint readUnsignedNumEntry( const QString &key, int defaultValue = 0 );
    QStringList readListEntry( const QString &key, QChar sep = ',' );
    bool readBoolEntry( const QString &key, bool defaultValue = false );
    QColor readColorEntry( const QString &key, const QColor *defaultVal );
    QDateTime readDateTimeEntry( const QString &key );

    QMap<QString,QString> entryMap( const QString &group ) { return m_backend->entryMap( group ); }

    void reparseConfiguration() { m_backend->load(); }

    void sync() { if ( !m_readOnly ) m_backend->save(); }

    void deleteGroup( const QString &group ) { m_backend->deleteGroup( group ); }

private:
    KConfigBackend *m_backend;
    bool m_readOnly;
};

class KConfigGroupSaver
{
public:
    KConfigGroupSaver( KConfig *config, const QString &group )
        : m_config( config ), m_oldGroup( config->group() )
    {
        m_config->setGroup( group );
    }
    ~KConfigGroupSaver()
    {
        m_config->setGroup( m_oldGroup );
    }

private:
    KConfig *m_config;
    QString m_oldGroup;
};

/*
 * vim:et
 */

#endif
