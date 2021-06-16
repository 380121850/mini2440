
#include <kio/global.h>

KIO::CacheControl KIO::parseCacheControl(const QString &cacheControl)
{
  QString tmp = cacheControl.lower();
 
  if (tmp == "cacheonly")
     return KIO::CC_CacheOnly;
  if (tmp == "cache")
     return KIO::CC_Cache;
  if (tmp == "verify")
     return KIO::CC_Verify;
  if (tmp == "reload")
     return KIO::CC_Reload;
 
  return KIO::CC_Verify;
}
 
QString KIO::getCacheControlString(KIO::CacheControl cacheControl)
{
    if (cacheControl == KIO::CC_CacheOnly)
        return "CacheOnly";
    if (cacheControl == KIO::CC_Cache)
        return "Cache";
    if (cacheControl == KIO::CC_Verify)
        return "Verify";
    if (cacheControl == KIO::CC_Reload)
        return "Reload";
    return QString::null;
}

