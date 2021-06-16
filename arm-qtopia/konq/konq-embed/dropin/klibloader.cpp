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

#include <config.h>

#include "klibloader.h"
#include "kglobal.h"
#include <kstddirs.h>

#include <assert.h>

#if defined(HAVE_DLFCN)
#include <dlfcn.h>
#endif

static QCString fileNameOf( const char *full )
{
    QCString name( full );
    int i = name.findRev( '/' );
    if ( i < 0 )
	i = name.findRev( '\\' );
    if ( i >= 0 )
	name = name.mid( i+1 );
    i = name.find( '.' );
    if ( i >= 0 )
	name.truncate( i );
    return name;
}

KLibrary::KLibrary( const QCString &name )
{
    m_name = name;
    m_factory = 0;
}

KLibrary::~KLibrary()
{
    // noone should ever destruct a KLibrary! (khtml_part.cpp tries to, but
    // it mustn't
    assert( false );
}

void KLibrary::registerSymbol( const char *key, void *symbol )
{
    m_symbols.replace( key, symbol );
}

void *KLibrary::symbol( const char *name )
{
    void *res = m_symbols[ name ];
    return res;
}

#if defined(HAVE_DLFCN)
class KDLOpenLibrary : public KLibrary
{
public:
    KDLOpenLibrary( const char *name, void *handle );
    ~KDLOpenLibrary();

    virtual void *symbol( const char *name );

private:
    void *m_handle;
};

KDLOpenLibrary::KDLOpenLibrary( const char *name, void *handle )
    : KLibrary( name ), m_handle( handle )
{
}

KDLOpenLibrary::~KDLOpenLibrary()
{
    // dlclose( handle );
}

void *KDLOpenLibrary::symbol( const char *name )
{
    return dlsym( m_handle, name );
}
#endif

KLibFactory *KLibrary::factory()
{
    if ( m_factory )
        return m_factory;

    void *sym = symbol( "init_" + m_name );

    if ( !sym )
        return 0;

    typedef KLibFactory * (*initFunc)();
    initFunc func = (initFunc)sym;

    m_factory = (*func)();

    return m_factory;
}

KLibrary *KLibLoader::library( const char *name, KLibLoader::LibraryOpeningPolicy policy )
{
    QCString fname = fileNameOf( name );
    KLibrary *lib = m_libs.find( fname );

    if ( !lib )
    {
	if ( policy == CreateStaticWrapper )
	{
	    lib = new KLibrary( fname );
	    m_libs.insert( fname, lib );
	}
#if defined(HAVE_DLFCN)
	else
	{
	    QCString path = findLibrary( name );
	    void *handle = dlopen( path, RTLD_LAZY );
	    if ( !handle )
	    {
		qDebug( "error opening %s: %s", path.data(), dlerror() );
		return 0;
	    }

	    lib = new KDLOpenLibrary( fname, handle );
	    m_libs.insert( fname, lib );
	}
#endif
    }

    return lib;
}

KLibrary *KLibLoader::globalLibrary( const char *name )
{
#if defined(HAVE_DLFCN)
    void *handle = dlopen( name, RTLD_LAZY | RTLD_GLOBAL );
    if ( !handle )
    {
	qDebug( "error opening %s: %s", name, dlerror() );
        return 0;
    }

    QCString fname = fileNameOf( name );
    KLibrary *lib = new KDLOpenLibrary( fname, handle );
    m_libs.insert( fname, lib );
    return lib;
#else
    return 0;
#endif
}

QCString KLibLoader::findLibrary( const QCString &name )
{
    if ( name.isEmpty() || name[ 0 ] == '/' )
	return name;

    QCString libName = name;
    // ### fixme: this is not portable
    if ( libName.findRev( '.' ) == -1 )
	libName.append( ".so" );

    QCString path = KGlobal::dirs()->findResource( "lib", libName ).local8Bit();
    if ( !path.isEmpty() )
	libName = path;

    return libName;
}

KLibLoader *KLibLoader::s_self = 0;

KLibLoader *KLibLoader::self()
{
    if ( !s_self )
        s_self = new KLibLoader;
    return s_self;
}

#include "klibloader.moc"
