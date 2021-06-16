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

#include "kconfig.h"
#include "kstddirs.h"
#include "kdebug.h"

#include <qfile.h>
#include <qtextstream.h>

#include <assert.h>

KConfigBackend::~KConfigBackend()
{
}

KConfigFileBackend::KConfigFileBackend( const QString &fileName )
    : m_fileName( fileName ), m_current( m_data.end() )
{
}

KConfigFileBackend::~KConfigFileBackend()
{
}

void KConfigFileBackend::load()
{
    m_data.clear();
    m_current = m_data.end();

    if ( m_fileName.isEmpty() )
        return;

    QStringList files = KGlobal::dirs()->findAllResources( "config",
                                                           m_fileName );
    QStringList::ConstIterator it = files.fromLast();
    QStringList::ConstIterator end = files.end();
    for (; it != end; --it )
    {
        //kdDebug() << "parsing " << *it << endl;
        parse( *it );
    }
}

void KConfigFileBackend::save()
{
    if ( m_fileName.isEmpty() )
        return;

    QString localFileName = m_fileName;
    if ( localFileName[0] != '/' )
        localFileName.prepend( KGlobal::dirs()->saveLocation( "config" ) );

    //kdDebug() << "writing to " << localFileName << endl;

    QFile f( localFileName );
    if ( !f.open( IO_WriteOnly ) )
        return;

    QTextStream s( &f );

    GroupMap::ConstIterator gIt = m_data.begin();
    GroupMap::ConstIterator gEnd = m_data.end();
    for (; gIt != gEnd; ++gIt )
    {
        EntryMap m = gIt.data();

        if ( m.isEmpty() ) // don't write empty groups
            continue;

        s << '[' << gIt.key() << ']' << endl;

        EntryMap::ConstIterator eIt = m.begin();
        EntryMap::ConstIterator eEnd = m.end();
        for (; eIt != eEnd; ++eIt )
            s << eIt.key() << '=' << eIt.data() << endl;
    }

    f.close();
}

QString KConfigFileBackend::group()
{
    if ( m_current == m_data.end() )
        return QString::null;
    return m_current.key();
}

void KConfigFileBackend::setGroup( const QString &group )
{
    QString grp = group;
    if ( grp.isEmpty() )
        grp = "<default>";

    m_current = m_data.find( grp );
    if ( m_current == m_data.end() )
        m_current = m_data.insert( grp, EntryMap() );
}

QStringList KConfigFileBackend::groupList() const
{
    QStringList res;
    GroupMap::ConstIterator it = m_data.begin();
    GroupMap::ConstIterator end = m_data.end();
    for (; it != end; ++it )
        res << it.key();

    return res;
}

bool KConfigFileBackend::hasGroup( const QString &group ) const
{
    return m_data.contains( group );
}

bool KConfigFileBackend::hasKey( const QString &key )
{
    if ( m_current == m_data.end() )
        return false;

    return m_current.data().contains( key );
}

QMap<QString, QString> KConfigFileBackend::entryMap( const QString &group )
{
    QString currentKey = m_current.key();

    GroupMap::ConstIterator grp = m_data.find( group );
    if ( grp == m_data.end() )
    {
        grp = m_data.insert( group, EntryMap() );
        m_current = m_data.find( currentKey );
    }

    return grp.data();
}

bool KConfigFileBackend::lookup( const QString &key, QString &value )
{
    if ( m_current == m_data.end() )
        return false;

    EntryMap::ConstIterator it = m_current.data().find( key );

    if ( it == m_current.data().end() )
        return false;

    value = *it;
    return true;
}

void KConfigFileBackend::put( const QString &key, const QString &value )
{
    if ( m_current != m_data.end() )
        m_current.data().replace( key, value );
    else
        kdDebug() << "writeEntry without previous setGroup!" << endl;
}

void KConfigFileBackend::deleteGroup( const QString &group )
{
    GroupMap::Iterator groupIt = m_data.find( group );

    if ( m_current == groupIt )
        m_current = m_data.end();

    m_data.remove( groupIt );
}

void KConfigFileBackend::parse( const QString &fileName )
{
    if ( fileName.isEmpty() )
        return;

    QFile f( fileName );
    if ( !f.open( IO_ReadOnly ) )
        return;

    QTextStream stream( &f );

    while ( !stream.eof() )
    {
        QString line = stream.readLine();

        QString stripped = line.stripWhiteSpace();

        // comment? skip
        if ( stripped[ 0 ] == '#' )
            continue;

        // group?
        if ( stripped[ 0 ] == '[' )
        {
            // find end of group
            int pos = stripped.find( ']' );

            if ( pos == -1 )
                continue;

            QString group = stripped.mid( 1, pos - 1 );

            m_current = m_data.find( group );
            if ( m_current == m_data.end() )
                m_current = m_data.insert( group, EntryMap() );
        }
        else if ( stripped.find( '=' ) != -1 )
        {
            uint pos = (uint)stripped.find( '=' );

            if ( m_current == m_data.end() )
            {
                // default group
                m_current = m_data.find( "<default>" );
                if ( m_current == m_data.end() )
                    m_current = m_data.insert( "<default>", EntryMap() );
            }

            if ( pos+1 == stripped.length() ) // need empty value
                m_current.data()[ stripped.left( pos ) ] = "";
            else
                m_current.data()[ stripped.left( pos ) ] = stripped.mid( pos+1 );
        }
    }
}

KConfig::KConfig( const QString &fileName, bool readOnly, bool )
    : m_backend( new KConfigFileBackend( fileName ) ),
      m_readOnly( readOnly )
{
    reparseConfiguration();
}

KConfig::KConfig( KConfigBackend *backEnd, bool readOnly )
    : m_backend( backEnd ), m_readOnly( readOnly )
{
    assert( m_backend );
}

KConfig::~KConfig()
{
    sync();
    delete m_backend;
}

void KConfig::writeEntry( const QString &key, const QString &value )
{
    m_backend->put( key, value );
}

void KConfig::writeEntry( const QString &key, int value )
{
    writeEntry( key, QString::number( value ) );
}

void KConfig::writeEntry( const QString &key, bool value )
{
    writeEntry( key, value ? QString::fromLatin1( "true" ) :
                             QString::fromLatin1( "false" ) );
}

void KConfig::writeEntry( const QString &key, const QStringList &value, QChar sep )
{
    QString val;

    QStringList::ConstIterator it = value.begin();
    QStringList::ConstIterator end = value.end();
    while ( it != end )
    {
        val += *it;
        ++it;
        if ( it != end )
            val += sep;
    }

    writeEntry( key, val );
}

void KConfig::writeEntry( const QString &key, const QDateTime &dateTime )
{
    QStringList list;

    QTime time = dateTime.time();
    QDate date = dateTime.date();

    list << QString::number( date.year() );
    list << QString::number( date.month() );
    list << QString::number( date.day() );

    list << QString::number( time.hour() );
    list << QString::number( time.minute() );
    list << QString::number( time.second() );

    writeEntry( key, list, ',' );
}

QString KConfig::readEntry( const QString &key, const QString &defaultValue )
{
    QString value;
    bool gotKey = m_backend->lookup( key, value );

    if ( !gotKey )
        return defaultValue;

    return value;
}

int KConfig::readNumEntry( const QString &key, int defaultValue )
{
    QString val = readEntry( key );

    if ( val.isEmpty() )
        return defaultValue;

    return val.toInt();
}

uint KConfig::readUnsignedNumEntry( const QString &key, int defaultValue )
{
    QString val = readEntry( key );

    if ( val.isEmpty() )
        return defaultValue;

    return val.toUInt();
}

QStringList KConfig::readListEntry( const QString &key, QChar sep )
{
    QString val = readEntry( key );

    if ( val.isEmpty() )
        return QStringList();

    return QStringList::split( sep, val );
}

bool KConfig::readBoolEntry( const QString &key, bool defaultValue )
{
    QString val = readEntry( key );

    if ( val.isEmpty() )
        return defaultValue;

    val = val.lower();

    if ( val == "true" )
        return true;

    if ( val == "on" )
        return true;

    if ( val == "off" )
        return false;

    if ( val == "false" )
        return false;

    return static_cast<bool>( val.toInt() );
}

QColor KConfig::readColorEntry( const QString &key, const QColor *defaultVal )
{
    QString val = readEntry( key );

    QColor res = *defaultVal;

    if ( !val.isEmpty() && val.at( 0 ) == '#' )
        res.setNamedColor( val );

    return res;
}

QDateTime KConfig::readDateTimeEntry( const QString &key )
{
    QDateTime res = QDateTime::currentDateTime();

    if( !hasKey( key ) )
        return res;

    QStringList list = readListEntry( key );

    if ( list.count() != 6 )
        return res;

    QTime time;
    QDate date;

    date.setYMD( list[ 0 ].toInt(),
                 list[ 1 ].toInt(),
                 list[ 2 ].toInt() );
    time.setHMS( list[ 3 ].toInt(),
                 list[ 4 ].toInt(),
                 list[ 5 ].toInt() );

    res.setTime( time );
    res.setDate( date );
    return res;
}

/*
 * vim:et
 */
