/*
	OTInetMailExchange.cc
	---------------------
*/

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Lamp
#include "lamp/OTInetMailExchange_k.h"


#pragma exceptions off


// Override ::OTInetMailExchange() to call into the kernel, so we can use
// our own SharedOpenTransport and InetSvcRef.
pascal OSStatus OTInetMailExchange( InetSvcRef,
                                    char              *domain,
                                    UInt16            *count,
                                    InetMailExchange  *result )
{
	return OTInetMailExchange_k( domain, count, result );
}

