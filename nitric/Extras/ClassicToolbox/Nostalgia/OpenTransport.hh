// Nostalgia/OpenTransport.hh

#ifndef NOSTALGIA_OPENTRANSPORT_HH
#define NOSTALGIA_OPENTRANSPORT_HH

#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if TARGET_API_MAC_CARBON
	
	inline pascal OSStatus InitOpenTransport()
	{
		return InitOpenTransportInContext( kInitOTForApplicationMask, NULL );
	}
	
	inline pascal void CloseOpenTransport()
	{
		CloseOpenTransportInContext( NULL );
	}
	
	inline pascal EndpointRef OTOpenEndpoint( OTConfigurationRef   config,
	                                          OTOpenFlags          flags,
	                                          TEndpointInfo       *info,
	                                          OSStatus            *err )
	{
		return OTOpenEndpointInContext( config, flags, info, err, NULL );
	}
	
#endif

#ifdef __cplusplus
}
#endif

#endif

