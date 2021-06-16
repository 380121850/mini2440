#ifndef __kparts_part_rw_h__
#define __kparts_part_rw_h__

#include <kparts/part.h>

namespace KParts
{
    class ReadWritePart : public KParts::ReadOnlyPart
    {
	Q_OBJECT
    public:
	ReadWritePart( QObject *parent = 0, const char *name = 0 )
	    : KParts::ReadOnlyPart( parent, name ), m_bModified( false ),
	      m_bReadWrite( true ) {}
	virtual ~ReadWritePart() {}
	virtual bool isReadWrite() const { return m_bReadWrite; }
	virtual void setReadWrite ( bool readwrite = true )
	    { m_bReadWrite = readwrite; }
	virtual bool isModified() const { return m_bModified; }
	virtual bool closeURL();
	virtual bool saveAs( const KURL &url );
	virtual void setModified( bool modified )
	    { m_bModified = m_bReadWrite && modified; }

    public slots:
	virtual void setModified() { setModified( true ); }
	virtual bool save();

    protected:
	virtual bool saveFile() = 0;
	virtual bool saveToURL();

    private:
	bool m_bModified;
	bool m_bReadWrite;
    };
};

#endif
