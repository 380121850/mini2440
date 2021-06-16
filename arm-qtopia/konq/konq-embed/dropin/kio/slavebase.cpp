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

#include "slavebase.h"
#include "job.h"

#include "http.h"
#include "kio_file.h"
#if defined(ENABLE_FTP)
#include "kio_ftp.h"
#endif

#include "slaveinterface.h"
#include "connection.h"

#include <qdatastream.h>
#include <qcstring.h>

#include <kconfig.h>
#include <kdebug.h>

#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>


using namespace KIO;

#define KIO_ARGS QByteArray data; QDataStream stream( data, IO_WriteOnly ); stream

class SlaveConfig : public KConfigBackend
{
public:
    SlaveConfig( SlaveBase *slave ) : m_slave( slave ) {}

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
    SlaveBase *m_slave;
};

void SlaveConfig::load()
{
}

void SlaveConfig::save()
{
}

QString SlaveConfig::group()
{
    return QString::null;
}

void SlaveConfig::setGroup( const QString & )
{
    assert( false );
}

QStringList SlaveConfig::groupList() const
{
    assert( false );
    return QStringList();
}

bool SlaveConfig::hasGroup( const QString & ) const
{
    assert( false );
    return false;
}

bool SlaveConfig::hasKey( const QString &key )
{
    return m_slave->hasMetaData( key );
}

QMap<QString, QString> SlaveConfig::entryMap( const QString & )
{
    assert( false );
    return QMap<QString, QString>();
}

bool SlaveConfig::lookup( const QString &key, QString &value )
{
    bool hasMetaData = m_slave->hasMetaData( key );
    if ( !hasMetaData )
        return false;

    value = m_slave->metaData( key );
    return true;
}

void SlaveConfig::put( const QString &, const QString & )
{
    assert( false );
}

void SlaveConfig::deleteGroup( const QString & )
{
    assert( false );
}

SlaveBase *SlaveBase::createSlave( const QString &prot )
{
    SlaveBase * res = 0;

    if ( false ) {}
    // Test first for externally defined protocols
    //  so we can override the standard ones
    else if ( File::implementsProto( prot ) )
        res = new File( prot );
    else if ( prot == "http" )
        res = new HTTPProtocol( prot.ascii(), "", "" );
#if !defined(Q_WS_WIN)
    else if ( prot == "https" )
        res = new HTTPProtocol( prot.ascii(), "", "" );
#endif
#if defined(ENABLE_FTP)
    else if ( prot == "ftp" )
        res = new FtpSlave;
#endif

    return res;
}

bool SlaveBase::knownProtocol( const QString &protocol )
{
    return ( protocol == "http" || protocol == "https" ||
#if defined(ENABLE_FTP)
	protocol == "ftp" ||
#endif
	File::implementsProto( protocol ) );
}

SlaveBase::SlaveBase( const QCString &protocol, const QCString &, const QCString & )
    : mProtocol( protocol ), m_dcc( new DCOPClient ), m_connection( 0 ),
      m_kconfig( new KConfig( new SlaveConfig( this ), true ) ), m_multipleAuthCaching( false )
{
}

SlaveBase::~SlaveBase()
{
    delete m_connection;
    delete m_dcc;
    delete m_kconfig;
}

void SlaveBase::dispatchLoop()
{
    assert( m_connection );

#if defined(Q_WS_WIN)
    while ( 42 )
    {
	QByteArray data;
	int cmd;
	m_connection->read( &cmd, data );
	if ( cmd == CMD_DISCONNECT )
       	    return;
	dispatch( cmd, data );
    }
#else
    fd_set fds;
    struct timeval tv;
    int retval;

    while ( 42 )
    {
        FD_ZERO( &fds );
        FD_SET( m_connection->fd_from(), &fds );

        tv.tv_sec = 30;
        tv.tv_usec = 0;

        retval = select( m_connection->fd_from() + 1, &fds, NULL, NULL, &tv );

        if ( retval > 0 )
        {
            if ( FD_ISSET( m_connection->fd_from(), &fds ) )
            {
                int cmd;
                QByteArray data;
                if ( m_connection->read( &cmd, data ) != -1 )
                    dispatch( cmd, data );
                else
                {
                    kdDebug() << "SlaveBase: read() error! exiting!" << endl;
                    ::exit( 0 );
                }
            }
        }
        else if ( retval == -1 )
        {
            kdDebug() << "SlaveBase: select() error! exiting!" << endl;
            ::exit( 0 );
        }
    }
#endif
}

void SlaveBase::data( const QByteArray &data )
{
    sendMetaData();
    m_connection->send( MSG_DATA, data );
}

void SlaveBase::dataReq()
{
    sendMetaData();

    m_connection->send( MSG_DATA_REQ );
}

void SlaveBase::error( int _errid, const QString &_text )
{
    m_incomingMetaData.clear();
    mOutgoingMetaData.clear();

    KIO_ARGS << _errid << _text;

    m_connection->send( MSG_ERROR, data );
}

void SlaveBase::connected()
{
}

void SlaveBase::finished()
{
    sendMetaData();
    m_connection->send( MSG_FINISHED );
}

void SlaveBase::slaveStatus( const QString &, bool )
{
}

void SlaveBase::statEntry( const UDSEntry & )
{
}

void SlaveBase::totalSize( unsigned long s )
{
    KIO_ARGS << s;
    m_connection->send( INF_TOTAL_SIZE, data );
}

void SlaveBase::processedSize( unsigned long s )
{
    KIO_ARGS << s;
    m_connection->send( INF_PROCESSED_SIZE, data );
}

void SlaveBase::speed( unsigned long )
{
}

void SlaveBase::redirection( const KURL &url )
{
    KIO_ARGS << url;
    m_connection->send( INF_REDIRECTION, data );
}

void SlaveBase::errorPage()
{
}

void SlaveBase::mimeType( const QString &_type )
{
    kdDebug(7019) << "(" << getpid() << ") SlaveBase::mimeType '" << _type << "'" << endl;
    if ( !mOutgoingMetaData.isEmpty() )
    {
	KIO_ARGS << mOutgoingMetaData;
	m_connection->send( INF_META_DATA, data );
    }

    KIO_ARGS << _type;
    m_connection->send( INF_MIME_TYPE, data );
}

void SlaveBase::listEntry( const UDSEntry &, bool )
{
}

void SlaveBase::infoMessage( const QString &msg )
{
    KIO_ARGS << msg;
    m_connection->send( INF_INFOMESSAGE, data );
}

bool SlaveBase::openPassDlg( AuthInfo &info )
{
    KIO_ARGS << info;

    int cmd;

    m_connection->send( INF_NEED_PASSWD, data );

    QByteArray replyData;

    if ( waitForAnswer( CMD_USERPASS, CMD_NONE, &cmd, replyData ) != -1 &&
         cmd == CMD_USERPASS )
    {
        AuthInfo res;
        QDataStream replyStream( replyData, IO_ReadOnly );
        replyStream >> res;

        info.username = res.username;
        info.password = res.password;
        info.keepPassword = res.keepPassword;
        return true;
    }

    return false;
}

int SlaveBase::messageBox( int type, const QString &text,
                const QString &caption,
                const QString &buttonYes,
                const QString &buttonNo )
{
    KIO_ARGS << type << text << caption << buttonYes << buttonNo;

    m_connection->send( INF_MESSAGEBOX, data );

    QByteArray replyData;

    if ( waitForAnswer( CMD_MESSAGEBOXANSWER, 0, 0, replyData ) != -1 )
    {
        QDataStream replyStream( replyData, IO_ReadOnly );
        int answer;

        replyStream >> answer;

        return answer;
    }
    else
    {
        assert( false );
        return 0;
    }
}

void SlaveBase::setMetaData( const QString &key, const QString &value )
{
    mOutgoingMetaData.insert( key, value );
}

bool SlaveBase::hasMetaData( const QString &key ) const
{
    if ( m_incomingMetaData.find( key ) != m_incomingMetaData.end() )
        return true;

    if ( m_config.find( key ) != m_config.end() )
        return true;

    return false;
}

QString SlaveBase::metaData( const QString &key ) const
{
    MetaData::ConstIterator it = m_incomingMetaData.find( key );

    if ( it == m_incomingMetaData.end() )
    {
        it = m_config.find( key );

        if ( it == m_config.end() )
            return QString::null;
    }

    return it.data();
}

void SlaveBase::setHost( const QString &, int , const QString &, const QString & )
{
}

void SlaveBase::get( const KURL & )
{
}

void SlaveBase::put( const KURL &, int , bool , bool )
{
}

void SlaveBase::stat( const KURL & )
{
}

void SlaveBase::mimetype( const KURL & )
{
}

void SlaveBase::special( const QByteArray & )
{
}

void SlaveBase::slave_status()
{
}

void SlaveBase::reparseConfiguration()
{
}

int SlaveBase::readData( QByteArray &buffer )
{
    return waitForAnswer( MSG_DATA, 0, 0, buffer );
}

bool SlaveBase::checkCachedAuthentication( AuthInfo &info )
{
   QCString replyType;
   QByteArray params, reply;
   QDataStream stream(params, IO_WriteOnly);
   QStringList result;

   stream << info.url.host() << info.realmValue;

   if (!m_dcc->call("authcache", "authcache",
        "searchAuthCache(QString,QString)", params, replyType, reply))
   {
      kdWarning(7103) << "Can't communicate with authcache!" << endl;
      return false;
   }

   QDataStream stream2(reply, IO_ReadOnly);
   if(replyType != "QStringList")
   {
      kdError(7103) << "DCOP function searchAuthCache(...) returns " << replyType << ", expected QStringList" << endl;
      return false;
   }

   stream2 >> result;
   if(result.isEmpty())
     return false;

   info.username = result[0];
   info.password = result[1];
   info.digestInfo = result[2];

   return true;
}

void SlaveBase::cacheAuthentication( const AuthInfo &info )
{
   QByteArray params;
   QDataStream stream(params, IO_WriteOnly);

   stream << info.url.host() << info.realmValue
	  << info.username << info.password << info.digestInfo;

   if (!m_dcc->send("authcache", "authcache",
        "storeAuthCache(QString,QString,QString,QString,QString)", params))
   {
      kdWarning(7103) << "Can't communicate with authcache!" << endl;
   }
}

void SlaveBase::sendMetaData()
{
    if ( mOutgoingMetaData.isEmpty() )
        return;

    KIO_ARGS << mOutgoingMetaData;
    m_connection->send( INF_META_DATA, data );
    mOutgoingMetaData.clear();
}

void SlaveBase::dispatch( int command, const QByteArray &packedArgs )
{
    QDataStream stream( packedArgs, IO_ReadOnly );

    KURL url;

    switch ( command )
    {
        case CMD_GET:
            stream >> url;
            get( url );
            break;
        case CMD_MIMETYPE:
            stream >> url;
            mimetype( url );
            break;
        case CMD_META_DATA:
            stream >> m_incomingMetaData;
            break;
        case CMD_SPECIAL:
            special( packedArgs );
            break;
        case CMD_HOST:
        {
            QString host, user, pass;
            Q_INT32 port;
            stream >> host >> port >> user >> pass;
            setHost( host, port, user, pass );
        }
        break;
        case CMD_CONFIG:
        {
            stream >> m_config;
        }
        break;
        case CMD_NONE:
	    break;
        default:
            assert( 0 );
    }
}

int SlaveBase::waitForAnswer( int expected1, int expected2, int *_cmd, QByteArray &data )
{
    int res;
    int cmd;

    while ( 42 )
    {
        res = m_connection->read( &cmd, data );

        if ( res == -1 )
            return res;

        if ( cmd == expected1 || cmd == expected2 )
        {
            if ( _cmd )
                *_cmd = cmd;
            return res;
        }

        // ###
        if ( cmd == CMD_META_DATA )
            dispatch( cmd, data );
        else
            kdDebug() << "received unexpected command " << cmd << " while actually waiting for " << expected1 << endl;
    }
}


