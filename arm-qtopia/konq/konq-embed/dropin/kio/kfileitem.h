#ifndef __kfileitem_h__
#define __kfileitem_h__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// ### ;-)
namespace KIO
{
    typedef struct stat UDSEntry;
};


class KFileItem
{
public:
    KFileItem( KIO::UDSEntry udsEntry, const KURL &, bool, bool )
        { m_entry = udsEntry; }

    bool isDir() const { return S_ISDIR( m_entry.st_mode ); }

private:
    KIO::UDSEntry m_entry;
};

#endif
