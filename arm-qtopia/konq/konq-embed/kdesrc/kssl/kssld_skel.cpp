/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from kssld.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "kssld.h"

#include <kdatastream.h>
#include <qasciidict.h>


static const int KSSLD_fhash = 13;
static const char* const KSSLD_ftable[12][3] = {
    { "void", "cacheAddCertificate(KSSLCertificate,KSSLCertificateCache::KSSLCertificatePolicy,bool)", "cacheAddCertificate(KSSLCertificate cert,KSSLCertificateCache::KSSLCertificatePolicy policy,bool permanent)" },
    { "KSSLCertificateCache::KSSLCertificatePolicy", "cacheGetPolicyByCN(QString)", "cacheGetPolicyByCN(QString cn)" },
    { "KSSLCertificateCache::KSSLCertificatePolicy", "cacheGetPolicyByCertificate(KSSLCertificate)", "cacheGetPolicyByCertificate(KSSLCertificate cert)" },
    { "bool", "cacheSeenCN(QString)", "cacheSeenCN(QString cn)" },
    { "bool", "cacheSeenCertificate(KSSLCertificate)", "cacheSeenCertificate(KSSLCertificate cert)" },
    { "bool", "cacheRemoveByCN(QString)", "cacheRemoveByCN(QString cn)" },
    { "bool", "cacheRemoveByCertificate(KSSLCertificate)", "cacheRemoveByCertificate(KSSLCertificate cert)" },
    { "bool", "cacheIsPermanent(KSSLCertificate)", "cacheIsPermanent(KSSLCertificate cert)" },
    { "void", "cacheReload()", "cacheReload()" },
    { "bool", "cacheModifyByCN(QString,KSSLCertificateCache::KSSLCertificatePolicy,bool,QDateTime)", "cacheModifyByCN(QString cn,KSSLCertificateCache::KSSLCertificatePolicy policy,bool permanent,QDateTime expires)" },
    { "bool", "cacheModifyByCertificate(KSSLCertificate,KSSLCertificateCache::KSSLCertificatePolicy,bool,QDateTime)", "cacheModifyByCertificate(KSSLCertificate cert,KSSLCertificateCache::KSSLCertificatePolicy policy,bool permanent,QDateTime expires)" },
    { 0, 0, 0 }
};

bool KSSLD::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    static QAsciiDict<int>* fdict = 0;
    if ( !fdict ) {
	fdict = new QAsciiDict<int>( KSSLD_fhash, TRUE, FALSE );
	for ( int i = 0; KSSLD_ftable[i][1]; i++ )
	    fdict->insert( KSSLD_ftable[i][1],  new int( i ) );
    }
    int* fp = fdict->find( fun );
    switch ( fp?*fp:-1) {
    case 0: { // void cacheAddCertificate(KSSLCertificate,KSSLCertificateCache::KSSLCertificatePolicy,bool)
	KSSLCertificate arg0;
	KSSLCertificateCache::KSSLCertificatePolicy arg1;
	bool arg2;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	replyType = KSSLD_ftable[0][0]; 
	cacheAddCertificate(arg0, arg1, arg2 );
    } break;
    case 1: { // KSSLCertificateCache::KSSLCertificatePolicy cacheGetPolicyByCN(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[1][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheGetPolicyByCN(arg0 );
    } break;
    case 2: { // KSSLCertificateCache::KSSLCertificatePolicy cacheGetPolicyByCertificate(KSSLCertificate)
	KSSLCertificate arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[2][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheGetPolicyByCertificate(arg0 );
    } break;
    case 3: { // bool cacheSeenCN(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[3][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheSeenCN(arg0 );
    } break;
    case 4: { // bool cacheSeenCertificate(KSSLCertificate)
	KSSLCertificate arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[4][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheSeenCertificate(arg0 );
    } break;
    case 5: { // bool cacheRemoveByCN(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[5][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheRemoveByCN(arg0 );
    } break;
    case 6: { // bool cacheRemoveByCertificate(KSSLCertificate)
	KSSLCertificate arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[6][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheRemoveByCertificate(arg0 );
    } break;
    case 7: { // bool cacheIsPermanent(KSSLCertificate)
	KSSLCertificate arg0;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	replyType = KSSLD_ftable[7][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheIsPermanent(arg0 );
    } break;
    case 8: { // void cacheReload()
	replyType = KSSLD_ftable[8][0]; 
	cacheReload( );
    } break;
    case 9: { // bool cacheModifyByCN(QString,KSSLCertificateCache::KSSLCertificatePolicy,bool,QDateTime)
	QString arg0;
	KSSLCertificateCache::KSSLCertificatePolicy arg1;
	bool arg2;
	QDateTime arg3;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	arg >> arg3;
	replyType = KSSLD_ftable[9][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheModifyByCN(arg0, arg1, arg2, arg3 );
    } break;
    case 10: { // bool cacheModifyByCertificate(KSSLCertificate,KSSLCertificateCache::KSSLCertificatePolicy,bool,QDateTime)
	KSSLCertificate arg0;
	KSSLCertificateCache::KSSLCertificatePolicy arg1;
	bool arg2;
	QDateTime arg3;
	QDataStream arg( data, IO_ReadOnly );
	arg >> arg0;
	arg >> arg1;
	arg >> arg2;
	arg >> arg3;
	replyType = KSSLD_ftable[10][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << cacheModifyByCertificate(arg0, arg1, arg2, arg3 );
    } break;
    default: 
	return KDEDModule::process( fun, data, replyType, replyData );
    }
    return TRUE;
}

QCStringList KSSLD::interfaces()
{
    QCStringList ifaces = KDEDModule::interfaces();
    ifaces += "KSSLD";
    return ifaces;
}

QCStringList KSSLD::functions()
{
    QCStringList funcs = KDEDModule::functions();
    for ( int i = 0; KSSLD_ftable[i][2]; i++ ) {
	QCString func = KSSLD_ftable[i][0];
	func += ' ';
	func += KSSLD_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


