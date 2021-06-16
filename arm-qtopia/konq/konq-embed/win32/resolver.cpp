
#include "resolver.h"

#include <assert.h>

#include <qtimer.h>

Resolver *Resolver::s_self = 0;

void Resolver::initStatic()
{
    assert( !s_self );
    s_self = new Resolver;
}

Resolver *Resolver::self()
{
    assert( s_self );
    return s_self;
}

bool Resolver::resolve( const QString &hostName, QHostAddress &address )
{
    NameRequest request( hostName );

    send( &request, Channel::WaitForReply );

    if ( request.resolved )
	address = request.address;

    return request.resolved;
}

void Resolver::receiveMessage( ChannelMessage *message, Channel::SendType )
{
    (void)new ResolveHelper( static_cast<NameRequest *>( message ) );
}

ResolveHelper::ResolveHelper( NameRequest *request )
    : m_dns( request->hostName ), m_request( request )
{
    connect( &m_dns, SIGNAL( resultsReady() ),
	     this, SLOT( nameResolved() ) );
}

void ResolveHelper::nameResolved()
{
    if ( !m_dns.addresses().isEmpty() )
    {
	m_request->address = m_dns.addresses()[ 0 ];
	m_request->resolved = true;
    }
    m_request->reply();
    QTimer::singleShot( 0, this, SLOT( deleteMe() ) );
}

void ResolveHelper::deleteMe()
{
    delete this;
}
