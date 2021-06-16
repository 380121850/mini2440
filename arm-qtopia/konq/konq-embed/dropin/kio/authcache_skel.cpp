/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from authcache.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "authcache.h"

#include <kdatastream.h>


static const char* const KAuthCacheServer_ftable[4][3] = {
    { "void", "storeAuthCache(QString,QString,QString,QString,QString)", "storeAuthCache(QString,QString,QString,QString,QString)" },
    { "QStringList", "searchAuthCache(QString,QString)", "searchAuthCache(QString,QString)" },
    { "void", "wipeAuthCache()", "wipeAuthCache()" },
    { 0, 0, 0 }
};

bool KAuthCacheServer::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if ( fun == KAuthCacheServer_ftable[0][1] ) { // void storeAuthCache(QString,QString,QString,QString,QString)
	QString arg0;
	QString arg1;
	QString arg2;
	QString arg3;
	QString arg4;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	arg >> arg3;
	arg >> arg4;
	replyType = KAuthCacheServer_ftable[0][0]; 
	storeAuthCache(arg0, arg1, arg2, arg3, arg4 );
    } else if ( fun == KAuthCacheServer_ftable[1][1] ) { // QStringList searchAuthCache(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	replyType = KAuthCacheServer_ftable[1][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << searchAuthCache(arg0, arg1 );
    } else if ( fun == KAuthCacheServer_ftable[2][1] ) { // void wipeAuthCache()
	replyType = KAuthCacheServer_ftable[2][0]; 
	wipeAuthCache( );
    } else {
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return TRUE;
}

QCStringList KAuthCacheServer::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "KAuthCacheServer";
    return ifaces;
}

QCStringList KAuthCacheServer::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; KAuthCacheServer_ftable[i][2]; i++ ) {
	QCString func = KAuthCacheServer_ftable[i][0];
	func += ' ';
	func += KAuthCacheServer_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


