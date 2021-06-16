/*  This file is part of the KDE project
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include "kapp.h"
#include "kconfig.h"
#include "dcopclient.h"
#include "kstddirs.h"

#include <unistd.h>
#include <qfile.h>
#include <assert.h>

KApplication *KApplication::s_self = 0;

KApplication::KApplication( int argc, char **argv, const char *name )
#ifdef _QT_QPE_
    : QPEApplication( argc, argv /*, name */ ), KInstance( name )
#else
    : QApplication( argc, argv, name ), KInstance( name )
#endif
{
    s_self = this;
    KGlobal::_instance = this; // ### hack

    m_dcopClient = new DCOPClient;
    KGlobal::dirs()->addResourceType("appdata", KStandardDirs::kde_default("data")
                                     + QString::fromLatin1(name) + '/');
}

KApplication::~KApplication()
{
    delete m_dcopClient;
    s_self = 0;
}

int KApplication::startServiceByDesktopName( const QString &, const QStringList &, QString * )
{
    return 0;
}

QString *KApplication::s_serviceName = 0;
void *KApplication::s_serviceEntryPoint = 0;

void KApplication::setService( const QString &desktopPath, void *entryPoint )
{
    if ( !s_serviceName )
        s_serviceName = new QString;

    *s_serviceName = desktopPath;
    s_serviceEntryPoint = entryPoint;
}

bool KApplication::startServiceByDesktopPath( const QString &path )
{
    // ### hack
    if ( s_serviceName && path == *s_serviceName && s_serviceEntryPoint )
    {
        if ( fork() == 0 )
        {
            typedef int (*EntryFunc)(int, char **);
            EntryFunc func = (EntryFunc)s_serviceEntryPoint;
            (*func)( qApp->argc(), qApp->argv() );
            ::exit( 0 );
        }

        return true;
    }

    return false;
}

bool checkAccess(const QString& pathname, int mode)
{
  int accessOK = access( QFile::encodeName(pathname), mode );
  if ( accessOK == 0 )
    return true;  // OK, I can really access the file

  // else
  // if we want to write the file would be created. Check, if the
  // user may write to the directory to create the file.
  if ( (mode & W_OK) == 0 )
    return false;   // Check for write access is not part of mode => bail out


  if (!access( QFile::encodeName(pathname), F_OK)) // if it already exists
      return false;

  //strip the filename (everything until '/' from the end
  QString dirName(pathname);
  int pos = dirName.findRev('/');
  if ( pos == -1 )
    return false;   // No path in argument. This is evil, we won't allow this

  dirName.truncate(pos); // strip everything starting from the last '/'

  accessOK = access( QFile::encodeName(dirName), W_OK );
  // -?- Can I write to the accessed diretory
  if ( accessOK == 0 )
    return true;  // Yes
  else
    return false; // No
}

#include "kapp.moc"
