#include <config.h>

#ifndef ENABLE_JS_HOSTEXTEND
#error You MUST configure with --enable-jshostext to use JS Debug Output
#endif

#include <jsextension.h>

#include <kjs/object.h>
#include <kjs/types.h>
#include <kjs/function.h>

#include <stdio.h>

using namespace KJS;

#if 1 // ANSI Supported?
static const char header[] = "\033[1;37m\033[43m\033[L"; // bold white on red
static const char footer[] = "\033[0m\033[L"; // reset attr & erase to EOL
#else
static const char header[] =
"--------------------------------------------------- Javascript DebugOut ---\n";
#define footer header
#endif

class JsDebug : public InternalFunctionImp
{
public:
    JsDebug() {}
    virtual Completion execute( const List &args );
};

Completion JsDebug::execute( const List &args )
{
    QString out( header );
    for ( int i = 0; i < args.size(); i++ )
    {
	out.append( args[i].toString().value().qstring() );
	out.append( "\n" );
    }
    out.append( footer );
    fputs( out.latin1(), stderr );
    return Completion( ReturnValue, Undefined() );
}

// One debug output is plenty enough for all interpreters
// As an added bonus it won't leak any more memory ;)
static JsDebug *s_debug = 0;

static void JsDebugExtension( KParts::ReadOnlyPart *part, const KURL &, const QString &, KJS::KJSO &hostobj )
{
    if ( !part->inherits( "KHTMLPart" ) )
	return;

    if ( !s_debug )
    {
	s_debug = new JsDebug;
	s_debug->ref();
    }

    hostobj.put( "DebugOut", KJSO( s_debug ) );
}

extern "C" void konqe_register_libjsdebug()
{
    KJS::addJsExtension( &JsDebugExtension );
}
