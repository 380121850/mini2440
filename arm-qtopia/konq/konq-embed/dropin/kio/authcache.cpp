/*

  Sam Revitch <samr7@cs.washington.edu>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <kdebug.h>
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kstddirs.h>
#include <unistd.h>
#include <qlist.h>

#include <dcopclient.h>
#include <kconfig.h>

#include "authcache.h"


// Authentication Cache handler
struct AuthInfoCache {
   QString host;		// Keys
   QString realm;

   QString username;		// Values
   QString password;
   QString digestInfo;

   int serial;			// Serial #, for expiration

   AuthInfoCache() { serial = 0; }

};

#define AUTH_CACHE_ENTRIES 16
static AuthInfoCache g_AuthCache[AUTH_CACHE_ENTRIES];


KAuthCacheServer::KAuthCacheServer()
{
}

KAuthCacheServer::~KAuthCacheServer()
{
}

// DCOP function
void KAuthCacheServer::storeAuthCache(QString host, QString realm,
				   QString user, QString pass,
				   QString digest)
{
  int i, si = -1, smallest = INT_MAX;
  QStringList strlist;

  if(user.isNull())
    return;

  for(i = 0; i < AUTH_CACHE_ENTRIES; i++) {
    if(smallest > g_AuthCache[i].serial)
      smallest = g_AuthCache[i].serial;
  }

  g_AuthCache[smallest].host = host;
  g_AuthCache[smallest].realm = realm;
  g_AuthCache[smallest].username = user;
  g_AuthCache[smallest].password = pass;
  g_AuthCache[smallest].digestInfo = digest;
}

// DCOP function
QStringList KAuthCacheServer::searchAuthCache(QString host, QString realm)
{
  int i, si = -1, largest = 0;
  QStringList strlist;

  for(i = 0; i < AUTH_CACHE_ENTRIES; i++) {
    if(largest < g_AuthCache[i].serial)
      largest = g_AuthCache[i].serial;

    if((si == -1) && (g_AuthCache[i].host == host)
       //       && (g_AuthCache[i].realm == realm)
       )
      si = i;
  }

  if(si != -1) {
    if(g_AuthCache[si].serial != largest)
      g_AuthCache[si].serial++;

    strlist.append(g_AuthCache[si].username);
    strlist.append(g_AuthCache[si].password);
    strlist.append(g_AuthCache[si].digestInfo);
  }

  return strlist;
}

// DCOP function
void KAuthCacheServer::wipeAuthCache()
{
  int i;

  for(i = 0; i < AUTH_CACHE_ENTRIES; i++) {
    g_AuthCache[i].host = (const char *) NULL;
    g_AuthCache[i].realm = (const char *) NULL;
    g_AuthCache[i].serial = 0;
  }
}


#include "authcache.moc"
