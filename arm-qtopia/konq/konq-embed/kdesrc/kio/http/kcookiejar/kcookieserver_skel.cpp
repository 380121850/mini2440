/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from kcookieserver.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "./kcookieserver.h"

#include <kdatastream.h>
#include <qasciidict.h>


static const int KCookieServer_fhash = 17;
static const char* const KCookieServer_ftable[16][3] = {
    { "QString", "findCookies(QString)", "findCookies(QString)" },
    { "QStringList", "findDomains()", "findDomains()" },
    { "QStringList", "findCookies(QValueList<int>,QString,QString,QString,QString)", "findCookies(QValueList<int>,QString,QString,QString,QString)" },
    { "QString", "findDOMCookies(QString)", "findDOMCookies(QString)" },
    { "void", "addCookies(QString,QCString,long int)", "addCookies(QString,QCString,long int)" },
    { "void", "deleteCookie(QString,QString,QString,QString)", "deleteCookie(QString,QString,QString,QString)" },
    { "void", "deleteCookiesFromDomain(QString)", "deleteCookiesFromDomain(QString)" },
    { "void", "deleteSessionCookies(long int)", "deleteSessionCookies(long int)" },
    { "void", "deleteSessionCookiesFor(QString,long int)", "deleteSessionCookiesFor(QString,long int)" },
    { "void", "deleteAllCookies()", "deleteAllCookies()" },
    { "void", "addDOMCookies(QString,QCString,long int)", "addDOMCookies(QString,QCString,long int)" },
    { "void", "setDomainAdvice(QString,QString)", "setDomainAdvice(QString,QString)" },
    { "QString", "getDomainAdvice(QString)", "getDomainAdvice(QString)" },
    { "void", "reloadPolicy()", "reloadPolicy()" },
    { "void", "shutdown()", "shutdown()" },
    { 0, 0, 0 }
};

bool KCookieServer::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    static QAsciiDict<int>* fdict = 0;
    if ( !fdict ) {
	fdict = new QAsciiDict<int>( KCookieServer_fhash, TRUE, FALSE );
	for ( int i = 0; KCookieServer_ftable[i][1]; i++ )
	    fdict->insert( KCookieServer_ftable[i][1],  new int( i ) );
    }
    int* fp = fdict->find( fun );
    switch ( fp?*fp:-1) {
    case 0: { // QString findCookies(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KCookieServer_ftable[0][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << findCookies(arg0 );
    } break;
    case 1: { // QStringList findDomains()
	replyType = KCookieServer_ftable[1][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << findDomains( );
    } break;
    case 2: { // QStringList findCookies(QValueList<int>,QString,QString,QString,QString)
	QValueList<int> arg0;
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
	replyType = KCookieServer_ftable[2][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << findCookies(arg0, arg1, arg2, arg3, arg4 );
    } break;
    case 3: { // QString findDOMCookies(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KCookieServer_ftable[3][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << findDOMCookies(arg0 );
    } break;
    case 4: { // void addCookies(QString,QCString,long int)
	QString arg0;
	QCString arg1;
	long int arg2;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	replyType = KCookieServer_ftable[4][0]; 
	addCookies(arg0, arg1, arg2 );
    } break;
    case 5: { // void deleteCookie(QString,QString,QString,QString)
	QString arg0;
	QString arg1;
	QString arg2;
	QString arg3;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	arg >> arg3;
	replyType = KCookieServer_ftable[5][0]; 
	deleteCookie(arg0, arg1, arg2, arg3 );
    } break;
    case 6: { // void deleteCookiesFromDomain(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KCookieServer_ftable[6][0]; 
	deleteCookiesFromDomain(arg0 );
    } break;
    case 7: { // void deleteSessionCookies(long int)
	long int arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KCookieServer_ftable[7][0]; 
	deleteSessionCookies(arg0 );
    } break;
    case 8: { // void deleteSessionCookiesFor(QString,long int)
	QString arg0;
	long int arg1;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	replyType = KCookieServer_ftable[8][0]; 
	deleteSessionCookiesFor(arg0, arg1 );
    } break;
    case 9: { // void deleteAllCookies()
	replyType = KCookieServer_ftable[9][0]; 
	deleteAllCookies( );
    } break;
    case 10: { // void addDOMCookies(QString,QCString,long int)
	QString arg0;
	QCString arg1;
	long int arg2;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	replyType = KCookieServer_ftable[10][0]; 
	addDOMCookies(arg0, arg1, arg2 );
    } break;
    case 11: { // void setDomainAdvice(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	replyType = KCookieServer_ftable[11][0]; 
	setDomainAdvice(arg0, arg1 );
    } break;
    case 12: { // QString getDomainAdvice(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KCookieServer_ftable[12][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << getDomainAdvice(arg0 );
    } break;
    case 13: { // void reloadPolicy()
	replyType = KCookieServer_ftable[13][0]; 
	reloadPolicy( );
    } break;
    case 14: { // void shutdown()
	replyType = KCookieServer_ftable[14][0]; 
	shutdown( );
    } break;
    default: 
	return KUniqueApplication::process( fun, data, replyType, replyData );
    }
    return TRUE;
}

QCStringList KCookieServer::interfaces()
{
    QCStringList ifaces = KUniqueApplication::interfaces();
    ifaces += "KCookieServer";
    return ifaces;
}

QCStringList KCookieServer::functions()
{
    QCStringList funcs = KUniqueApplication::functions();
    for ( int i = 0; KCookieServer_ftable[i][2]; i++ ) {
	QCString func = KCookieServer_ftable[i][0];
	func += ' ';
	func += KCookieServer_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


