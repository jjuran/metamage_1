/*
	OTInetMailExchange.cc
	---------------------
*/

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Lamp
#include "lamp/_OTInetMailExchange.h"


#pragma exceptions off


// Override ::OTInetMailExchange() to call into the kernel, so we can use
// our own SharedOpenTransport and InetSvcRef.
pascal OSStatus OTInetMailExchange( InetSvcRef,
                                    char              *domain,
                                    UInt16            *count,
                                    InetMailExchange  *result )
{
	return _OTInetMailExchange( domain, count, result );
}

