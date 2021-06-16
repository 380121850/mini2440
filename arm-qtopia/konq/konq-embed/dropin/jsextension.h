#ifndef __jsextension_h__
#define __jsextension_h__

#if defined(ENABLE_JS_HOSTEXTEND)

#include <kjs/object.h>
#include <kparts/part.h>
#include <qstring.h>

typedef void (*JsHostExtend)( KParts::ReadOnlyPart *part, const KURL &url,
			      const QString &mimetype, KJS::KJSO &hostobj );

namespace KJS {

    void addJsExtension( const JsHostExtend ext );

    KJSO externalObject( KJSO globobj = Global::current() );

};

#endif // ENABLE_JS_HOSTEXTEND

#endif // __jsextension_h__
