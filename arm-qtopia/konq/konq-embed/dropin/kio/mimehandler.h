#ifndef __kio_mimehandler_h__
#define __kio_mimehandler_h__

#include <kio/global.h>
#include <kparts/factory.h>

namespace KIO
{

    class DataFilter : public QObject
    {
        Q_OBJECT
    public:
	virtual ~DataFilter() {}
	virtual const QString& outputType() const = 0;

    public slots:
        virtual void dataInput( const QByteArray &dat ) = 0;

    signals:
        void dataOutput( const QByteArray &data );
    };


    class ExternalFilter : public DataFilter
    {
        Q_OBJECT
    public:
	static ExternalFilter* Create( const QString &app, const QString& type, const KURL &url );
	virtual ~ExternalFilter();
	virtual const QString& outputType() const { return m_output; }

    public slots:
        virtual void dataInput( const QByteArray &dat );

    signals:
        void dataOutput( const QByteArray &data );

    private:
	ExternalFilter( int _infile, int _outfile, int _pid, const QString &otype )
	    : m_file_in( _infile ), m_file_out( _outfile ), m_pid( _pid ),
	      m_output( otype ) {}
	bool pumpData();
	void closeOutput();
	int m_file_in;
	int m_file_out;
	int m_pid;
	QString m_output;
    };


    class MimeInternal;
    class MimePart;
    class MimeUnknown;
    class MimeAlias;
    class MimeFilter;
    class MimeExtApp;


    class MimeHandler : public QObject
    {
    public:
	static void Initialize();
	static void AddHandler( const MimeHandler *handler )
	    { s_handlers.insert( 0, handler ); }
	MimeHandler( const QStringList &types ) { m_types = types; }
	static const MimeHandler* Find( const QString &type );
	static const MimeHandler* Find( const KURL &url );
	static const MimeHandler* Find( const QByteArray &data );

	const QStringList& mimeTypes() const { return m_types; }
	const QString& Preferred() const { return m_types[0]; }
	virtual const MimeInternal* isInternal() const { return 0; }
	virtual const MimePart* isPart() const { return 0; }
	virtual const MimeUnknown* isUnknown() const { return 0; }
	virtual const MimeAlias* isAlias() const { return 0; }
	virtual const MimeFilter* isFilter() const { return 0; }
	virtual const MimeExtApp* isExtApp() const { return 0; }

    protected:
	virtual int Matches( const QString &type ) const;
	virtual int Matches( const KURL &url ) const;
	virtual bool Matches( const QByteArray &/*data*/ ) const  { return false; }
	QStringList m_types;

	static QList<MimeHandler> s_handlers;
    };


    class MimeInternal : public MimeHandler
    {
    public:
	MimeInternal( const QStringList &types ) : MimeHandler( types ) {}
	virtual const MimeInternal* isInternal() const { return this; }
    };


    class MimeInternalImage : public MimeInternal
    {
    public:
	MimeInternalImage( const QStringList &types ) : MimeInternal( types ) {}
    protected:
	virtual bool Matches( const QByteArray &data ) const;
    };


    class MimePart : public MimeHandler
    {
    public:
	MimePart( const QStringList &types, KParts::Factory *factory, bool readwrite = false )
	    : MimeHandler( types ), m_factory( factory ), m_lib( QString::null ), m_rw( readwrite ) {}
	MimePart( const QStringList &types, const QString &library, bool readwrite = false )
	    : MimeHandler( types ), m_factory( 0 ), m_lib( library ), m_rw( readwrite ) {}
	virtual const MimePart* isPart() const { return this; }
	KParts::Factory* getFactory();
	bool isReadWrite() const { return m_rw; }
    protected:
	KParts::Factory* m_factory;
	QString m_lib;
	bool m_rw;
    };


    class MimeUnknown : public MimeHandler
    {
    public:
	MimeUnknown( const QStringList &types ) : MimeHandler( types ) {}
	virtual const MimeUnknown* isUnknown() const { return this; }
    };


    class MimeAlias : public MimeHandler
    {
    public:
	MimeAlias( const QStringList &types, const QString &otype )
	    : MimeHandler( types ) { m_output = otype; }
	virtual const MimeAlias* isAlias() const { return this; }
	virtual const QString& outputType() const { return m_output; }

    protected:
	QString m_output;
    };


    class MimeFilter : public MimeHandler
    {
    public:
	MimeFilter( const QStringList &types ) : MimeHandler( types ) {}
	virtual const MimeFilter* isFilter() const { return this; }
	virtual DataFilter* createFilter( const QString& type, const KURL &url ) const = 0;
	virtual const QString& outputType() const = 0;
    };


    class MimeExtFilter : public MimeFilter
    {
    public:
	MimeExtFilter( const QStringList &types, const QString &path, const QString &otype )
	    : MimeFilter( types ) { m_path = path; m_output = otype; }
	virtual DataFilter* createFilter ( const QString& type, const KURL &url ) const;
	virtual const QString& outputType() const { return m_output; }

    protected:
	QString m_path;
	QString m_output;
    };


    class MimeExtApp : public MimeHandler
    {
    public:
	MimeExtApp( const QStringList &types, const QString &path, bool wantdata )
	    : MimeHandler( types ) { m_path = path; m_wantdata = wantdata; }

	virtual const MimeExtApp* isExtApp() const { return this; }
	QString getExtApp() const { return m_path; }
	bool wantsData() const { return m_wantdata; }

    protected:
	QString m_path;
	bool m_wantdata;
    };

};

#endif
