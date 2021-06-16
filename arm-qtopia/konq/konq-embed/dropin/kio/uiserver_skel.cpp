/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from uiserver.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "uiserver.h"

#include <kdatastream.h>


static const char* const UIServer_ftable[2][3] = {
    { "void", "showSSLInfoDialog(QString,KIO::MetaData)", "showSSLInfoDialog(QString url,KIO::MetaData data)" },
    { 0, 0, 0 }
};

bool UIServer::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if ( fun == UIServer_ftable[0][1] ) { // void showSSLInfoDialog(QString,KIO::MetaData)
	QString arg0;
	KIO::MetaData arg1;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	replyType = UIServer_ftable[0][0]; 
	showSSLInfoDialog(arg0, arg1 );
    } else {
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return TRUE;
}

QCStringList UIServer::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "UIServer";
    return ifaces;
}

QCStringList UIServer::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; UIServer_ftable[i][2]; i++ ) {
	QCString func = UIServer_ftable[i][0];
	func += ' ';
	func += UIServer_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


