#include <config.h>

#if defined(ENABLE_JS_HOSTEXTEND)

#include "jsextension.h"
#include "jsextconnect.h"

#include <kjs/object.h>
#include <kjs/types.h>

#include <qlist.h>

static QList<void> js_extensions;

namespace KJS {

    void addJsExtension( const JsHostExtend ext )
    {
	js_extensions.append( (void *)ext );
    }

    void probeJsExtension( KParts::ReadOnlyPart *part, const KURL &url,
			   const QString &mimetype, Imp *imp )
    {
	QListIterator<void> it( js_extensions );
	Global global( Global::current() );
	void *oldextra = global.extra();
	if ( !oldextra )
	    global.setExtra( part );

	KJS::KJSO hostobj( imp ? imp : global.imp() );
	for ( ; it.current(); ++it )
	    ( (JsHostExtend)(it.current()) ) ( part, url, mimetype, hostobj );

	if ( !oldextra )
	    global.setExtra( 0 );
    }

    KJSO externalObject( KJSO globobj )
    {
	KJSO ext = globobj.get( "external" );
	if ( !ext.isObject() )
	{
	    ext = Object( new HostImp );
	    globobj.put( "external", ext );
	}
	return ext;
    }

};

#endif // ENABLE_JS_HOSTEXTEND
